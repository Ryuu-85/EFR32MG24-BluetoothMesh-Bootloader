#include "app.h"

#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "sl_status.h"
#include "app_timer.h"
#include "zigbee_transfer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MESH_UNPROV_BEARERS     0x03U
#define MESH_PRIMARY_ELEMENT    0U
#define MESH_VENDOR_ID          0x02FFU
#define MESH_VENDOR_MODEL_ID    0x0001U
#define MESH_VENDOR_OPCODE_DATA 0x01U
#define MESH_DEFAULT_DST_ADDR   0xC000U
#define MESH_NETKEY_INDEX       0x0000U
#define MESH_APPKEY_INDEX       0x0000U
#define MESH_SELF_PROVISIONING  1U
#define MESH_IV_INDEX           0U
#define MESH_MIN_UNICAST_ADDR   0x0100U
#define MESH_UNICAST_ADDR_SPAN  0x7E00U

static const aes_key_128 mesh_test_netkey = {
  .data = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
  }
};

static const aes_key_128 mesh_test_appkey = {
  .data = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
  }
};

static bool mesh_node_initialized;
static bool mesh_node_provisioned;
static bool mesh_has_netkey;
static bool mesh_has_appkey;
static bool mesh_model_configured;
static bool mesh_subscription_configured;
static bool mesh_vendor_model_initialized;
static uint16_t mesh_primary_address;
static uint16_t mesh_appkey_index;
static uint16_t mesh_destination_address = MESH_DEFAULT_DST_ADDR;
static uint32_t mesh_iv_index;
static app_timer_t mesh_reset_timer;

static char to_lower_ascii(char value)
{
  if (value >= 'A' && value <= 'Z') {
    return (char)(value - 'A' + 'a');
  }
  return value;
}

static const char *model_config_state_name(uint8_t state)
{
  switch (state) {
    case 0:
      return "appkey bind";
    case 1:
      return "publication";
    case 2:
      return "subscription";
    default:
      return "unknown";
  }
}

static void print_hex_payload(const uint8_t *data, uint16_t length)
{
  for (uint16_t index = 0U; index < length; index++) {
    sl_iostream_printf(sl_iostream_get_default(), "%02X ", data[index]);
  }
}

static bool parse_hex_u16(const char *text, uint16_t *value)
{
  char *end = NULL;
  unsigned long parsed;

  if (text == NULL || value == NULL) {
    return false;
  }

  while (*text == ' ' || *text == '\t') {
    text++;
  }

  if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    text += 2;
  }

  parsed = strtoul(text, &end, 16);
  if (end == text || parsed > 0xFFFFUL) {
    return false;
  }

  *value = (uint16_t)parsed;
  return true;
}

static void on_mesh_reset_timer(app_timer_t *handle, void *data)
{
  (void)handle;
  (void)data;
  sl_bt_system_reset(0);
}

static void schedule_system_reset(void)
{
  sl_status_t status = app_timer_start(&mesh_reset_timer,
                                       500U,
                                       on_mesh_reset_timer,
                                       NULL,
                                       false);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh] system reset scheduled, sc=0x%04lx\r\n",
                     (unsigned long)status);
}

static void init_vendor_model(void)
{
  const uint8_t opcodes[] = { MESH_VENDOR_OPCODE_DATA };
  sl_status_t status;

  if (mesh_vendor_model_initialized) {
    return;
  }

  status = sl_btmesh_vendor_model_init(MESH_PRIMARY_ELEMENT,
                                       MESH_VENDOR_ID,
                                       MESH_VENDOR_MODEL_ID,
                                       0,
                                       sizeof(opcodes),
                                       opcodes);
  mesh_vendor_model_initialized = (status == SL_STATUS_OK);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh vendor] init cid=0x%04x model=0x%04x opcode=0x%02x sc=0x%04lx\r\n",
                     MESH_VENDOR_ID,
                     MESH_VENDOR_MODEL_ID,
                     MESH_VENDOR_OPCODE_DATA,
                     (unsigned long)status);
}

static void discover_existing_appkey(void)
{
  uint32_t appkey_count = 0U;
  uint16_t appkey_id = 0U;
  uint16_t netkey_index = 0U;
  aes_key_128 appkey;
  sl_status_t count_status;
  sl_status_t key_status;

  count_status = sl_btmesh_node_get_key_count(sl_btmesh_node_key_type_app,
                                              &appkey_count);
  if (count_status != SL_STATUS_OK || appkey_count == 0U) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh cfg] appkey discovery count=%lu sc=0x%04lx\r\n",
                       (unsigned long)appkey_count,
                       (unsigned long)count_status);
    return;
  }

  key_status = sl_btmesh_node_get_key(sl_btmesh_node_key_type_app,
                                      0U,
                                      1U,
                                      &appkey_id,
                                      &netkey_index,
                                      &appkey);
  if (key_status == SL_STATUS_OK) {
    mesh_has_appkey = true;
    mesh_appkey_index = appkey_id;
  } else if (appkey_count > 0U) {
    mesh_has_appkey = true;
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh cfg] appkey index read failed, fallback to appkey=0x%04x\r\n",
                       (unsigned int)mesh_appkey_index);
  }

  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh cfg] appkey discovery count=%lu appkey=0x%04x netkey=0x%04x sc=0x%04lx\r\n",
                     (unsigned long)appkey_count,
                     (unsigned int)appkey_id,
                     (unsigned int)netkey_index,
                     (unsigned long)key_status);
}

static bool mesh_has_valid_netkey(void)
{
  uint32_t netkey_count = 0U;
  sl_status_t status;

  status = sl_btmesh_node_get_key_count(sl_btmesh_node_key_type_net,
                                        &netkey_count);
  mesh_has_netkey = (status == SL_STATUS_OK && netkey_count > 0U);

  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh cfg] netkey discovery count=%lu sc=0x%04lx\r\n",
                     (unsigned long)netkey_count,
                     (unsigned long)status);

  return mesh_has_netkey;
}

static bool mesh_local_config_status_ok(sl_status_t status)
{
  return status == SL_STATUS_OK
         || status == SL_STATUS_ALREADY_EXISTS
         || status == SL_STATUS_BT_MESH_ALREADY_EXISTS;
}

static void self_provision_mesh_node(void)
{
  uuid_128 uuid;
  aes_key_128 device_key;
  uint16_t address;
  uint16_t uuid_tail;
  sl_status_t uuid_status;
  sl_status_t stop_status;
  sl_status_t provision_status;

  uuid_status = sl_btmesh_node_get_uuid(&uuid);
  if (uuid_status != SL_STATUS_OK) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh self] cannot read UUID, sc=0x%04lx\r\n",
                       (unsigned long)uuid_status);
    return;
  }

  memcpy(device_key.data, uuid.data, sizeof(device_key.data));
  uuid_tail = ((uint16_t)uuid.data[14] << 8) | uuid.data[15];
  address = MESH_MIN_UNICAST_ADDR
            + (uint16_t)(uuid_tail % MESH_UNICAST_ADDR_SPAN);

  stop_status = sl_btmesh_node_stop_unprov_beaconing();
  if (stop_status != SL_STATUS_OK
      && stop_status != SL_STATUS_BT_MESH_DOES_NOT_EXIST) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh self] stop beacon failed, sc=0x%04lx\r\n",
                       (unsigned long)stop_status);
    return;
  }

  provision_status = sl_btmesh_node_set_provisioning_data(device_key,
                                                          mesh_test_netkey,
                                                          MESH_NETKEY_INDEX,
                                                          MESH_IV_INDEX,
                                                          address,
                                                          0U);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh self] provision addr=0x%04x sc=0x%04lx; rebooting\r\n",
                     (unsigned int)address,
                     (unsigned long)provision_status);
  if (provision_status == SL_STATUS_OK) {
    schedule_system_reset();
  }
}

static void configure_vendor_model(void)
{
  sl_status_t key_status;
  sl_status_t bind_status;
  sl_status_t sub_status;

  if (!mesh_node_initialized || !mesh_node_provisioned) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh cfg] node not ready for model configuration\r\n");
    return;
  }

  key_status = sl_btmesh_test_add_local_key(sl_btmesh_test_key_type_app,
                                            mesh_test_appkey,
                                            MESH_APPKEY_INDEX,
                                            MESH_NETKEY_INDEX);
  mesh_has_appkey = mesh_local_config_status_ok(key_status);
  mesh_appkey_index = MESH_APPKEY_INDEX;
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh self] add appkey=0x%04x sc=0x%04lx\r\n",
                     MESH_APPKEY_INDEX,
                     (unsigned long)key_status);
  if (!mesh_has_appkey) {
    return;
  }

  bind_status = sl_btmesh_test_bind_local_model_app(MESH_PRIMARY_ELEMENT,
                                                    MESH_APPKEY_INDEX,
                                                    MESH_VENDOR_ID,
                                                    MESH_VENDOR_MODEL_ID);
  mesh_model_configured = mesh_local_config_status_ok(bind_status);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh self] bind vendor model appkey=0x%04x sc=0x%04lx\r\n",
                     MESH_APPKEY_INDEX,
                     (unsigned long)bind_status);

  sub_status = sl_btmesh_test_add_local_model_sub(MESH_PRIMARY_ELEMENT,
                                                  MESH_VENDOR_ID,
                                                  MESH_VENDOR_MODEL_ID,
                                                  mesh_destination_address);
  mesh_subscription_configured = mesh_local_config_status_ok(sub_status);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh self] subscribe dst=0x%04x sc=0x%04lx\r\n",
                     (unsigned int)mesh_destination_address,
                     (unsigned long)sub_status);
}

static void print_uuid(void)
{
  uuid_128 uuid;
  sl_status_t status = sl_btmesh_node_get_uuid(&uuid);

  sl_iostream_printf(sl_iostream_get_default(), "[mesh] uuid status=0x%04lx",
                     (unsigned long)status);
  if (status == SL_STATUS_OK) {
    sl_iostream_printf(sl_iostream_get_default(), ", value=");
    for (uint8_t index = 0U; index < sizeof(uuid.data); index++) {
      sl_iostream_printf(sl_iostream_get_default(), "%02X", uuid.data[index]);
    }
  }
  sl_iostream_printf(sl_iostream_get_default(), "\r\n");
}

static void print_mesh_status(void)
{
  uint32_t netkey_count = 0U;
  uint32_t appkey_count = 0U;
  sl_status_t netkey_status;
  sl_status_t appkey_status;

  netkey_status = sl_btmesh_node_get_key_count(sl_btmesh_node_key_type_net,
                                               &netkey_count);
  appkey_status = sl_btmesh_node_get_key_count(sl_btmesh_node_key_type_app,
                                               &appkey_count);

  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh] status init=%u provisioned=%u addr=0x%04x iv=0x%08lx netkeys=%lu(sc=0x%04lx) appkeys=%lu(sc=0x%04lx) vendor_init=%u model_cfg=%u sub_cfg=%u appkey=0x%04x dst=0x%04x\r\n",
                     (unsigned int)mesh_node_initialized,
                     (unsigned int)mesh_node_provisioned,
                     (unsigned int)mesh_primary_address,
                     (unsigned long)mesh_iv_index,
                     (unsigned long)netkey_count,
                     (unsigned long)netkey_status,
                     (unsigned long)appkey_count,
                     (unsigned long)appkey_status,
                     (unsigned int)mesh_vendor_model_initialized,
                     (unsigned int)mesh_model_configured,
                     (unsigned int)mesh_subscription_configured,
                     (unsigned int)mesh_appkey_index,
                     (unsigned int)mesh_destination_address);
}

static void start_unprovisioned_beaconing(void)
{
  sl_status_t status;

  if (!mesh_node_initialized) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh] node not initialized, cannot beacon yet\r\n");
    return;
  }

  if (mesh_node_provisioned) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh] already provisioned, beacon not started\r\n");
    return;
  }

  status = sl_btmesh_node_start_unprov_beaconing(MESH_UNPROV_BEARERS);
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh] start unprovisioned beaconing PB-ADV+PB-GATT, sc=0x%04lx\r\n",
                     (unsigned long)status);
}

static void reset_mesh_node(void)
{
  sl_status_t status;

  if (mesh_node_provisioned) {
    status = sl_btmesh_node_reset();
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh] factory reset requested, sc=0x%04lx\r\n",
                       (unsigned long)status);
    schedule_system_reset();
  } else {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh] unprovisioned already, rebooting\r\n");
    sl_bt_system_reset(0);
  }
}

static bool line_matches(const char *line, const char *expected)
{
  return strcmp(line, expected) == 0;
}

static void print_command_help(void)
{
  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh cmd] !status | !uuid | !bind | !dst c000 | !appkey 0000 | !beacon | !reset | !help\r\n");
}

static void app_network_state_changed(network_state_t state)
{
  switch (state) {
    case NETWORK_STATE_DISCONNECTED:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[app] network disconnected\r\n");
      break;
    case NETWORK_STATE_CONNECTED:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[app] network connected\r\n");
      break;
    case NETWORK_STATE_JOINED:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[app] network joined/ready\r\n");
      break;
    default:
      break;
  }
}

static void app_command_received(uint16_t source_node,
                                 uint8_t *data,
                                 uint8_t length)
{
  (void)source_node;
  (void)data;
  (void)length;
}

void app_init(void)
{
  sl_iostream_printf(sl_iostream_get_default(),
                     "\r\n[app] Bluetooth Mesh SDK 4.5.0 bridge start\r\n");
  print_command_help();

  zigbee_transfer_init();
  zigbee_transfer_register_network_handler(app_network_state_changed);
  zigbee_transfer_register_command_handler(app_command_received);
}

void app_process_action(void)
{
  zigbee_transfer_process_rx();
}

void sl_bt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      sl_iostream_printf(sl_iostream_get_default(), "[bt] system boot\r\n");
      break;
    default:
      break;
  }
}

void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      mesh_node_initialized = true;
      mesh_node_provisioned = evt->data.evt_node_initialized.provisioned;
      mesh_primary_address = evt->data.evt_node_initialized.address;
      mesh_iv_index = evt->data.evt_node_initialized.iv_index;

      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] node initialized, provisioned=%u, addr=0x%04x, iv=0x%08lx\r\n",
                         (unsigned int)mesh_node_provisioned,
                         (unsigned int)mesh_primary_address,
                         (unsigned long)mesh_iv_index);
      init_vendor_model();
      if (mesh_node_provisioned) {
        if (!mesh_has_valid_netkey()) {
          sl_iostream_printf(sl_iostream_get_default(),
                             "[mesh] invalid provisioned state, resetting mesh node\r\n");
          reset_mesh_node();
          break;
        }
        discover_existing_appkey();
        configure_vendor_model();
      }
      print_uuid();

      if (!mesh_node_provisioned) {
#if MESH_SELF_PROVISIONING
        sl_iostream_printf(sl_iostream_get_default(),
                           "[mesh self] development self-provisioning enabled\r\n");
        self_provision_mesh_node();
#else
        sl_iostream_printf(sl_iostream_get_default(),
                           "[mesh] waiting for provisioner, PB-ADV+PB-GATT enabled\r\n");
#endif
      }
      break;

    case sl_btmesh_evt_node_provisioning_started_id:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] provisioning started, result=0x%04x\r\n",
                         evt->data.evt_node_provisioning_started.result);
      break;

    case sl_btmesh_evt_node_provisioning_failed_id:
      mesh_node_provisioned = false;
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] provisioning failed, result=0x%04x\r\n",
                         evt->data.evt_node_provisioning_failed.result);
      start_unprovisioned_beaconing();
      break;

    case sl_btmesh_evt_node_provisioned_id:
      mesh_node_provisioned = true;
      mesh_primary_address = evt->data.evt_node_provisioned.address;
      mesh_iv_index = evt->data.evt_node_provisioned.iv_index;
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] node provisioned, addr=0x%04x, iv=0x%08lx\r\n",
                         (unsigned int)mesh_primary_address,
                         (unsigned long)mesh_iv_index);
      break;

    case sl_btmesh_evt_node_key_added_id:
      if (evt->data.evt_node_key_added.type == sl_btmesh_node_key_type_net) {
        mesh_has_netkey = true;
      } else if (evt->data.evt_node_key_added.type == sl_btmesh_node_key_type_app) {
        mesh_has_appkey = true;
        mesh_appkey_index = evt->data.evt_node_key_added.index;
      }
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] key added type=%u index=0x%04x netkey=0x%04x\r\n",
                         (unsigned int)evt->data.evt_node_key_added.type,
                         (unsigned int)evt->data.evt_node_key_added.index,
                         (unsigned int)evt->data.evt_node_key_added.netkey_index);
      break;

    case sl_btmesh_evt_node_model_config_changed_id:
      if (evt->data.evt_node_model_config_changed.node_config_state == 0U
          && evt->data.evt_node_model_config_changed.vendor_id == MESH_VENDOR_ID
          && evt->data.evt_node_model_config_changed.model_id == MESH_VENDOR_MODEL_ID) {
        mesh_model_configured = true;
      } else if (evt->data.evt_node_model_config_changed.node_config_state == 2U
                 && evt->data.evt_node_model_config_changed.vendor_id == MESH_VENDOR_ID
                 && evt->data.evt_node_model_config_changed.model_id == MESH_VENDOR_MODEL_ID) {
        mesh_subscription_configured = true;
      }
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] model config changed state=%s elem=0x%04x vendor=0x%04x model=0x%04x\r\n",
                         model_config_state_name(evt->data.evt_node_model_config_changed.node_config_state),
                         (unsigned int)evt->data.evt_node_model_config_changed.element_address,
                         (unsigned int)evt->data.evt_node_model_config_changed.vendor_id,
                         (unsigned int)evt->data.evt_node_model_config_changed.model_id);
      break;

    case sl_btmesh_evt_node_config_get_id:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] config get netkey=0x%04x id=0x%04x\r\n",
                         (unsigned int)evt->data.evt_node_config_get.netkey_index,
                         (unsigned int)evt->data.evt_node_config_get.id);
      break;

    case sl_btmesh_evt_node_config_set_id:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] config set netkey=0x%04x id=0x%04x len=%u\r\n",
                         (unsigned int)evt->data.evt_node_config_set.netkey_index,
                         (unsigned int)evt->data.evt_node_config_set.id,
                         (unsigned int)evt->data.evt_node_config_set.value.len);
      break;

    case sl_btmesh_evt_node_reset_id:
      mesh_node_initialized = false;
      mesh_node_provisioned = false;
      mesh_has_netkey = false;
      mesh_has_appkey = false;
      mesh_model_configured = false;
      mesh_subscription_configured = false;
      mesh_vendor_model_initialized = false;
      mesh_primary_address = 0U;
      mesh_appkey_index = 0U;
      mesh_iv_index = 0U;
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] node reset event, system will reboot\r\n");
      break;

    case sl_btmesh_evt_node_ivrecovery_needed_id:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh] iv recovery needed node_iv=0x%08lx network_iv=0x%08lx\r\n",
                         (unsigned long)evt->data.evt_node_ivrecovery_needed.node_iv_index,
                         (unsigned long)evt->data.evt_node_ivrecovery_needed.network_iv_index);
      break;

    case sl_btmesh_evt_vendor_model_receive_id:
      if (evt->data.evt_vendor_model_receive.vendor_id == MESH_VENDOR_ID
          && evt->data.evt_vendor_model_receive.model_id == MESH_VENDOR_MODEL_ID
          && evt->data.evt_vendor_model_receive.opcode == MESH_VENDOR_OPCODE_DATA) {
        sl_iostream_printf(sl_iostream_get_default(),
                           "[mesh vendor rx] src=0x%04x dst=0x%04x appkey=0x%04x final=%u len=%u hex=",
                           (unsigned int)evt->data.evt_vendor_model_receive.source_address,
                           (unsigned int)evt->data.evt_vendor_model_receive.destination_address,
                           (unsigned int)evt->data.evt_vendor_model_receive.appkey_index,
                           (unsigned int)evt->data.evt_vendor_model_receive.final,
                           (unsigned int)evt->data.evt_vendor_model_receive.payload.len);
        print_hex_payload(evt->data.evt_vendor_model_receive.payload.data,
                          evt->data.evt_vendor_model_receive.payload.len);
        sl_iostream_printf(sl_iostream_get_default(), "\r\n");
        zigbee_transfer_mesh_to_serial(evt->data.evt_vendor_model_receive.payload.data,
                                       evt->data.evt_vendor_model_receive.payload.len);
      }
      break;

    case sl_btmesh_evt_vendor_model_send_complete_id:
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh vendor tx] complete handle=0x%04x result=0x%04x flags=0x%02x\r\n",
                         (unsigned int)evt->data.evt_vendor_model_send_complete.handle,
                         (unsigned int)evt->data.evt_vendor_model_send_complete.result,
                         (unsigned int)evt->data.evt_vendor_model_send_complete.message_flags);
      break;

    default:
      break;
  }
}

bool app_handle_serial_command(const uint8_t *data, uint16_t length)
{
  char line[64];
  char *command = line;
  uint16_t copy_length;

  if (data == NULL || length == 0U) {
    return false;
  }

  copy_length = length < (sizeof(line) - 1U) ? length : (sizeof(line) - 1U);
  for (uint16_t index = 0U; index < copy_length; index++) {
    line[index] = to_lower_ascii((char)data[index]);
  }
  line[copy_length] = '\0';

  while (*command == ' ' || *command == '\t') {
    command++;
  }

  if (*command == '!') {
    command++;
  } else if (strncmp(command, "mesh ", 5U) == 0) {
    command += 5;
  } else if (!line_matches(command, "status")
             && !line_matches(command, "uuid")
             && !line_matches(command, "beacon")
             && !line_matches(command, "reset")
             && !line_matches(command, "help")
             && !line_matches(command, "bind")
             && strncmp(command, "dst", 3U) != 0
             && strncmp(command, "appkey", 6U) != 0) {
    return false;
  }

  while (*command == ' ' || *command == '\t') {
    command++;
  }

  if (line_matches(command, "status")) {
    print_mesh_status();
  } else if (line_matches(command, "uuid")) {
    print_uuid();
  } else if (line_matches(command, "beacon")) {
    start_unprovisioned_beaconing();
  } else if (line_matches(command, "reset")) {
    reset_mesh_node();
  } else if (line_matches(command, "help")) {
    print_command_help();
  } else if (line_matches(command, "bind")) {
    configure_vendor_model();
  } else if (strncmp(command, "dst", 3U) == 0) {
    uint16_t address;
    if (parse_hex_u16(command + 3, &address)) {
      mesh_destination_address = address;
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh cmd] destination set to 0x%04x\r\n",
                         (unsigned int)mesh_destination_address);
    } else {
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh cmd] bad destination, example: !dst c000\r\n");
    }
  } else if (strncmp(command, "appkey", 6U) == 0) {
    uint16_t appkey_index;
    if (parse_hex_u16(command + 6, &appkey_index)) {
      mesh_appkey_index = appkey_index;
      mesh_has_appkey = true;
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh cmd] appkey index set to 0x%04x\r\n",
                         (unsigned int)mesh_appkey_index);
    } else {
      sl_iostream_printf(sl_iostream_get_default(),
                         "[mesh cmd] bad appkey, example: !appkey 0000\r\n");
    }
  } else {
    return false;
  }

  return true;
}

void send_mesh_data(uint8_t *data, uint16_t length)
{
  if (data == NULL || length == 0U) {
    return;
  }

  if (!mesh_node_initialized) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh send] node not initialized yet\r\n");
    return;
  }

  if (!mesh_node_provisioned) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh send] node not provisioned yet\r\n");
    return;
  }

  if (!mesh_vendor_model_initialized) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh send] vendor model not initialized\r\n");
    return;
  }

  if (!mesh_has_appkey || !mesh_model_configured) {
    sl_iostream_printf(sl_iostream_get_default(),
                       "[mesh send] vendor model is not bound/configured yet, appkey=%u model_cfg=%u\r\n",
                       (unsigned int)mesh_has_appkey,
                       (unsigned int)mesh_model_configured);
    return;
  }

  sl_status_t status;
  uint16_t handle = 0U;

  status = sl_btmesh_vendor_model_send_tracked(mesh_destination_address,
                                              -1,
                                              mesh_appkey_index,
                                              MESH_PRIMARY_ELEMENT,
                                              MESH_VENDOR_ID,
                                              MESH_VENDOR_MODEL_ID,
                                              0,
                                              0,
                                              MESH_VENDOR_OPCODE_DATA,
                                              1,
                                              length,
                                              data,
                                              &handle);

  sl_iostream_printf(sl_iostream_get_default(),
                     "[mesh vendor tx] dst=0x%04x appkey=0x%04x len=%u handle=0x%04x sc=0x%04lx\r\n",
                     (unsigned int)mesh_destination_address,
                     (unsigned int)mesh_appkey_index,
                     (unsigned int)length,
                     (unsigned int)handle,
                     (unsigned long)status);
}
