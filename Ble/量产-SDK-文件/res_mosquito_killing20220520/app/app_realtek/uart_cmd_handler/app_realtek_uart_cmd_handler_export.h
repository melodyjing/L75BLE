extern void cmd_dev_pair_mode_handler(USHORT length,UCHAR *user_data);
extern void cmd_dev_mac_addr_handler(USHORT length,UCHAR *user_data);
extern void cmd_ble_params_handler(USHORT length,UCHAR *user_data);
extern void report_up_rsp(enum MODUL_INFO info_index,UINT8 *data);
extern UINT8 get_state_report(void);
void cmd_ble_conn_info_handler(enum UART_COMMAND_TYPE type);
extern void cmd_firmware_version_read_handler(void);
extern void cmd_filter_addr_handler(USHORT length,UCHAR *user_data);
extern void cmd_filter_type_handler(USHORT length,UCHAR *user_data);
extern void cmd_adv_info_report_handler(USHORT length,UCHAR *user_data);
extern void cmd_uart_params_handler(USHORT length,UCHAR *user_data);
extern void cmd_get_filter_info_read_handler(void);
extern void cmd_pd_Handler(USHORT length,UCHAR *user_data);
extern void cmd_module_info_handler(USHORT length,UCHAR *user_data);
extern void cmd_adv_data_handler(USHORT length,UCHAR *user_data);
extern void cmd_resq_data_handler(USHORT length,UCHAR *user_data);
#if BLE_DATA_RECEIVE_MODE
void cmd_ble_data_receive_mode_handler(USHORT length,UCHAR *user_data);
void app_set_ble_data_rece_mode(enum BLE_DATA_RECE_MODE mode);
#endif
