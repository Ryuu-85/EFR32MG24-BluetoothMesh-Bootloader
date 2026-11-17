#ifndef ZIGBEE_TRANSFER_H
#define ZIGBEE_TRANSFER_H

#include <stdint.h>

typedef enum {
  NETWORK_STATE_DISCONNECTED = 0,
  NETWORK_STATE_CONNECTED,
  NETWORK_STATE_JOINED
} network_state_t;

typedef void (*zigbee_command_callback_t)(uint16_t source_node,
                                          uint8_t *data,
                                          uint8_t length);
typedef void (*network_event_callback_t)(network_state_t state);

void zigbee_transfer_init(void);
void zigbee_transfer_process_rx(void);
void zigbee_transfer_mesh_to_serial(uint8_t *data, uint8_t length);
void zigbee_transfer_register_command_handler(zigbee_command_callback_t handler);
void zigbee_transfer_register_network_handler(network_event_callback_t handler);
void zigbee_transfer_send_to_target(uint8_t *data, uint8_t length);
void send_mesh_data(uint8_t *data, uint16_t length);

#endif
