#include "zigbee_transfer.h"

#include "app.h"
#include "sl_iostream.h"
#include "sl_status.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RX_BUFFER_SIZE 128U

static bool initialized;
static bool joined;
static zigbee_command_callback_t command_handler;
static network_event_callback_t network_handler;
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_length;

static void print_hex(const uint8_t *data, uint16_t length)
{
  uint16_t index;

  for (index = 0; index < length; index++) {
    sl_iostream_printf(sl_iostream_get_default(), "%02X ", data[index]);
  }
}

static void print_text(const uint8_t *data, uint16_t length)
{
  uint16_t index;

  sl_iostream_printf(sl_iostream_get_default(), "\"");
  for (index = 0U; index < length; index++) {
    uint8_t character = data[index];

    if (character == '\\' || character == '"') {
      sl_iostream_printf(sl_iostream_get_default(), "\\%c", character);
    } else if (character >= 0x20U && character <= 0x7EU) {
      sl_iostream_printf(sl_iostream_get_default(), "%c", character);
    } else {
      sl_iostream_printf(sl_iostream_get_default(), "\\x%02X", character);
    }
  }
  sl_iostream_printf(sl_iostream_get_default(), "\"");
}

void zigbee_transfer_init(void)
{
  initialized = true;
  joined = true;
  rx_length = 0U;

  sl_iostream_printf(sl_iostream_get_default(),
                     "[zigbee_transfer] init ok\r\n");
  sl_iostream_printf(sl_iostream_get_default(),
                     "[zigbee_transfer] type text and press Enter\r\n");
}

void zigbee_transfer_process_rx(void)
{
  uint8_t character = 0U;
  size_t bytes_read = 0U;
  sl_status_t status;

  if (!initialized) {
    return;
  }

  status = sl_iostream_read(sl_iostream_get_default(),
                            &character,
                            1U,
                            &bytes_read);
  if (status != SL_STATUS_OK || bytes_read == 0U) {
    return;
  }

  (void)sl_iostream_write(sl_iostream_get_default(), &character, 1U);

  if (character == '\r' || character == '\n') {
    if (rx_length > 0U) {
      sl_iostream_printf(sl_iostream_get_default(),
                         "\r\n[serial rx] len=%u, hex=",
                         (unsigned int)rx_length);
      print_hex(rx_buffer, rx_length);
      sl_iostream_printf(sl_iostream_get_default(), "\r\n");
      if (!app_handle_serial_command(rx_buffer, rx_length)) {
        zigbee_transfer_send_to_target(rx_buffer, (uint8_t)rx_length);
      }
      rx_length = 0U;
    }
    return;
  }

  if (rx_length < RX_BUFFER_SIZE) {
    rx_buffer[rx_length++] = character;
  } else {
    sl_iostream_printf(sl_iostream_get_default(),
                       "\r\n[serial rx] buffer full, clear\r\n");
    rx_length = 0U;
  }
}

void zigbee_transfer_mesh_to_serial(uint8_t *data, uint8_t length)
{
  if (!initialized || data == NULL || length == 0U) {
    return;
  }

  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh rx -> serial] len=%u, text=",
                     (unsigned int)length);
  print_text(data, length);
  sl_iostream_printf(sl_iostream_get_default(), ", hex=");
  print_hex(data, length);
  sl_iostream_printf(sl_iostream_get_default(), "\r\n");

  if (command_handler != NULL) {
    command_handler(0xFFFFU, data, length);
  }
}

void zigbee_transfer_register_command_handler(zigbee_command_callback_t handler)
{
  command_handler = handler;
}

void zigbee_transfer_register_network_handler(network_event_callback_t handler)
{
  network_handler = handler;
  if (network_handler != NULL) {
    network_handler(joined ? NETWORK_STATE_JOINED
                           : NETWORK_STATE_DISCONNECTED);
  }
}

void zigbee_transfer_send_to_target(uint8_t *data, uint8_t length)
{
  if (!initialized || !joined || data == NULL || length == 0U) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[zigbee_transfer] not ready, cannot send\r\n");
    return;
  }

  sl_iostream_printf(sl_iostream_get_default(),
                     "[serial -> mesh] len=%u, hex=",
                     (unsigned int)length);
  print_hex(data, length);
  sl_iostream_printf(sl_iostream_get_default(), "\r\n");
  send_mesh_data(data, length);
}
