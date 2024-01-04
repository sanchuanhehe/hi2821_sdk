OUTPUT_ARCH(riscv)
ENTRY(_start)
MEMORY
{
    ROM : ORIGIN = (0x10000 + 0x5400), LENGTH = 0x2ac00
    ITCM : ORIGIN = (0x40000), LENGTH = (0x14000)
    DTCM : ORIGIN = 0x20000000, LENGTH = ((0x20000000 + 0x10000 - 0x100) - ((((0x20000000 + 0x400) + 0x800) + 0x200)))
    PRESERVE : ORIGIN = (0x20000000 + 0x10000 - 0x100), LENGTH = 0x100
    FLASH_STARTUP : ORIGIN = ((0x90100000) + (10 * 4096) + 4096 + 0x300 + (10 * 4096)), LENGTH = 0x300
    FLASH_PROGRAM : ORIGIN = ((0x90100000) + (10 * 4096) + 4096 + 0x300 + (10 * 4096)) + 0x300, LENGTH = ((0x90100000 + 0x80000) - ((0x90100000) + (10 * 4096) + 4096 + 0x300 + (10 * 4096)) - 2 * 4096 - 0 * 4096) - 0x300
    CPUTRACE_RAM : ORIGIN = 0x52006000, LENGTH = 0x3FC
}
SECTIONS
{
    . = 0;
    __CODE_START__ = ORIGIN(FLASH_STARTUP);
    startup :
    {
        KEEP (*(.text.entry))
        . = ALIGN(8);
    } > FLASH_STARTUP
    .version_str :
    {
        . = ALIGN(8);
        KEEP (*(.flash_version))
        . = ALIGN(8);
    } > FLASH_STARTUP
    __program_size__ =.;
    .stacks (NOLOAD): ALIGN(16)
    {
        g_system_stack_begin = .;
        . += 0x400;
        . = ALIGN(16);
        __stack_top__ = .;
        . += 0x800;
        . = ALIGN(16);
        __irq_stack_top__ = .;
        __irq_stack_top = .;
        . += 0x200;
        . = ALIGN(16);
        __excp_stack_top__ = .;
        __exc_stack_top = .;
        . += 0x200;
        . = ALIGN(16);
        __nmi_stack_top__ = .;
        __nmi_stack_top = .;
        g_system_stack_end = .;
        . += 0xC00;
        . = ALIGN(16);
    } > DTCM
    g_system_stack_size = g_system_stack_end - g_system_stack_begin;
    g_stack_system = g_system_stack_size;
}
SECTIONS
{
    .btc_romtext :
    {
        . = ALIGN(8);
        KEEP(SORT(*libbgtp_rom.a:)(.text .text* .rodata .rodata* .srodata .srodata*))
        . = ALIGN(8);
    } > ROM
    .bth_romtext :
    {
        . = ALIGN(8);
        KEEP(SORT(*libbg_common_rom*.a:sysdep*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*libbt_host_rom*.a:)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*libbg_common_rom*.a:)(.text .text* .rodata .rodata* .srodata .srodata*))
        . = ALIGN(8);
    } > ROM
    .plt_romtext :
    {
        . = ALIGN(8);
        KEEP(SORT(*memset_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*memcpy_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*memmove_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*strcpy_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*strncpy_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*strcat_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*strncat_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*strtok_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*sprintf_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*snprintf_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*vsprintf_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*vsnprintf_s*.o*)(.text .text* .rodata .rodata* .srodata .srodata*)) KEEP(SORT(*secureprintoutput_a*.o*)(.text .text* .rodata .rodata* .srodata .srodata*))
        . = ALIGN(8);
    } > ROM
}
SECTIONS
{
    .patch_data :
    {
        . = ALIGN(8);
        __patch_data_load__ = LOADADDR(.patch_data);
        __patch_data_begin__ = .;
        KEEP(*(.patch_remap))
        KEEP(*(.patch_cmp))
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    __patch_data_end__ = .;
    __patch_data_size__ = __patch_data_end__ - __patch_data_begin__;
    .sysintcall.init : {
        . = ALIGN(4);
        __sysinitcall_start = .;
        __sysinitcall0_start = .;
        KEEP(*(SORT(.sysinitcall_level0_*)))
        __sysinitcall1_start = .;
        KEEP(*(SORT(.sysinitcall_level1_*)))
        __sysinitcall2_start = .;
        KEEP(*(SORT(.sysinitcall_level2_*)))
        __sysinitcall3_start = .;
        KEEP(*(SORT(.sysinitcall_level3_*)))
        __sysinitcall4_start = .;
        KEEP(*(SORT(.sysinitcall_level4_*)))
        __sysinitcall5_start = .;
        KEEP(*(SORT(.sysinitcall_level5_*)))
        __sysinitcall6_start = .;
        KEEP(*(SORT(.sysinitcall_level6_*)))
        __sysinitcall7_start = .;
        KEEP(*(SORT(.sysinitcall_level7_*)))
        __sysinitcall_end = .;
        . = ALIGN(4);
    } > FLASH_PROGRAM
    .rom_ram_bss :
    {
        __romram_bss_begin__ = .;
        . = ALIGN(32);
        _SVENEER_ROM_RAM_B = .;KEEP(*.o*(.bss.g_al_acc_val_i));KEEP(*.o*(.bss.g_al_acc_val_q));KEEP(*.o*(.bss.g_config_align_onebyte));KEEP(*.o*(.bss.g_config_att_trnas_save_copy));KEEP(*.o*(.bss.g_config_fixed_public_secret_key));KEEP(*.o*(.bss.g_config_gap_le_features_init_ready));KEEP(*.o*(.bss.g_config_gap_use_static_addr));KEEP(*.o*(.bss.g_config_le_eatt_service_support));KEEP(*.o*(.bss.g_config_le_master_use_privacy));KEEP(*.o*(.bss.g_config_le_runtime_service_change));KEEP(*.o*(.bss.g_config_le_slave_use_privacy));KEEP(*.o*(.bss.g_dm_co_desc));KEEP(*.o*(.bss.g_es_env));KEEP(*.o*(.bss.g_gatt_appearance_category_type));KEEP(*.o*(.bss.g_l2cap_cid_userset));KEEP(*.o*(.bss.g_sm3_triggered));; _EVENEER_ROM_RAM_B = .;
        . = ALIGN(8);
    } > ITCM
    __romram_bss_end__ = .;
    __romram_bss_size__ = __romram_bss_end__ - __romram_bss_begin__;
    .rom_ram_text :
    {
        __romtext_load__ = LOADADDR(.rom_ram_text);
        __romtext_begin__ = .;
        . = ALIGN(8);
        _SVENEER_TEXT = .; . = ALIGN(8); KEEP(*.o*(.__ashldi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__divdi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__udivdi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__umoddi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.atoi.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_handle_range_ind_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_item_check_security_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_item_find_check_pdu_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_pendind_cbk_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_tl_disconnect_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.att_tl_hci_lelink_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgsub_pmu_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_get_lowpower_clock_freq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_sleep_irq_clear.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_sleep_time_calc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_soc_wakeup_irq_clear.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_start_sleep.romcall.text)); . = ALIGN(8); KEEP(*.o*(.ble_acl_calc_drift.romcall.text)); . = ALIGN(8); KEEP(*.o*(.ble_sca_get_cfg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bt_init_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bt_rf_convert_accum_to_mv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bt_rf_exec_accum_imm.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_delay.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_get_att_sec_keyword.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_get_gatt_device_name.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_get_gatt_device_name_len.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_hci_malloc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_is_gap_name_update.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bth_is_pts_mode.romcall.text)); . = ALIGN(8); KEEP(*.o*(.btos_dep_done.romcall.text)); . = ALIGN(8); KEEP(*.o*(.btos_dep_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.btos_free.romcall.text)); . = ALIGN(8); KEEP(*.o*(.btos_new.romcall.text)); . = ALIGN(8); KEEP(*.o*(.btos_new_s.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_ble_chnl_cls_to_chnl_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_get_valid_map_num.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_gle_2m_phy_chnl_map_from_1m.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_gle_4m_phy_chnl_map_from_1m.romcall.text)); . = ALIGN(8); KEEP(*.o*(.close_event.romcall.text)); . = ALIGN(8); KEEP(*.o*(.compress_printf.romcall.text)); . = ALIGN(8); KEEP(*.o*(.create_critical.romcall.text)); . = ALIGN(8); KEEP(*.o*(.create_event.romcall.text)); . = ALIGN(8); KEEP(*.o*(.create_thread.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_dft_data_ext_global_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_local_feats.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_local_states.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_local_supp_cmds_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_local_supp_feats_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_reset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_gle_get_desc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_gle_get_host_chnl_cls_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_gle_get_local_version.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_gle_hci_gle_basic_cmp_send.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_gle_send_status_evt.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_sleep_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_veto_sys_sleep.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_veto_sys_wakeup.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_wakeup_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_intc_delete.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_intc_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_disable_int.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_enable_int.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_intc_delete.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_intc_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_hci_write_to_self.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_interrupt_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_lowpower_feature_get.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_random.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_sec_random.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_trigger_crypto_engine.romcall.text)); . = ALIGN(8); KEEP(*.o*(.encrypt_clear_sm3_tasks.romcall.text)); . = ALIGN(8); KEEP(*.o*(.encrypt_init_sm3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.enter_critical.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_finetimer_get_first_target_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_finetimer_prog_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_ee_type.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_es_type.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_priority_by_prio_idx.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_prog_pop_elt_match_isr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_prog_program_eeq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_check_offset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_create_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_get_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_get_offset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_insert_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_remove_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_resched_check.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_scan_update_param.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_trigger.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_trigger_cancel.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_update_acl_param_with_instant.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_llcp_ack_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_llcp_desc_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_unknown_llcp_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_refresh_llcp_instant_ram.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_update_active_tx_octets.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_create.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_get_dur_tbl_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_get_intv_tbl_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_set_adv_en.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_config_extheader_syncinfo.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_config_txpld_get_extheader_flag.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_set_adv_en.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_initiate_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_initiate_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_scan_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_scan_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_create_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_env_get.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_env_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_fsm_except_handler.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_rx_error_record.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_task_end.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_task_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_add_instant_prog.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_check_instant_pass.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_check_mcs.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_check_pre_instant_pass.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_effective_data_length.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_env.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_first_instant.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_interval.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_phy_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_pre_instant_clk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_get_tx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_set_mcs.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_set_mic_trx_filter.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_set_phy_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_update_active_tx_octets_before_len_req.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_gle_acb_update_effective_data_length.romcall.text)); . = ALIGN(8); KEEP(*.o*(.free_critical.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_event_func_external.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_event_func_internal.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_free.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_get_random.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_inst_free.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_inst_new_.romcall.text)); . = ALIGN(8); KEEP(*.o*(.fsm_set_random.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gap_acllink_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gap_le_disable_periodic_advertising.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gap_service_cbk_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gap_vendor_command_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_acl_link_disconnect_chr_report.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_init_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_internal_cbk_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_check_repeated_request_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_ext_adv_enable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_multi_adv_data.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_multi_adv_enable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_multi_adv_param.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_multi_random_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_prd_adv_enable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_cmd_send_set_random_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_extended_scan_period_check.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_le_mgr_read_controller_features.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_lemgr_cleanup_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_lemgr_done_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gaph_smp_cbk_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_done_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_general_attribute_serivce_add_characters_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_init_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_item_write_check_pdu_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_item_write_error_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_pendind_single_response_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_prepare_write_cfm_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_prepare_write_ind_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_reg_character_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_reg_descriptor_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_reg_include_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_reg_service_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_server_create_update_ccc_item.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_service_add_characters_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_task_list_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_tl_data_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_tl_done_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_tl_hci_lelink_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gatt_tl_l2cap_cbk_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gattc_pendind_response_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.gattc_tl_data_ind_process_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_att_hci_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_att_hci_cbk_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_comm.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_comm_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_le.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_le_prime.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_le_prime_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_gap_hci_cbk_le_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_l2cap_hci_ev_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_l2cap_hci_ev_cbk_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_smp_hci_ev_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.get_smp_hci_ev_cbk_size.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_acl_data_send.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_acl_data_send_pendding.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_acl_link_kill.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_acl_link_ref.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_acl_link_un_ref.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_available_addr_cap2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_ble_rx_acl_data.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_bt_send_evt_msg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_bt_send_evt_msg_without_log.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cbk_prime_reg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cbk_reg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cbk_un_reg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cbk_un_reg_sync.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cleanup.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cmd_rmv_err_func.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_cmd_send_func.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_command_check_support.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_command_send_amp.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_command_send_vendor.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_compare_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_compare_all_num.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_controller_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_disconnect.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_done.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_gle_send_evt_msg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_info_read_local.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_link_remove.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_local_changed_notify.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_local_find_by_id.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_local_start_timer.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_page0feature_find_by_remote_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_remote_find_by_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_remote_find_create.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_remote_get_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_remote_new_cap_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_security_get_bondable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_security_get_local_io.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_security_set_bondable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_security_set_local_io.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_security_set_oob.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_tl_reg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_tl_un_reg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hcic_security_le_keys_recover.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hcic_security_le_keys_remove.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hcih_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_ble_tx_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_ble_tx_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_gle_tx_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_gle_tx_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_tx_s_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_tx_s_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_tx_desc_get_txpd_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_tx_desc_get_txpd_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.init_schedule.romcall.text)); . = ALIGN(8); KEEP(*.o*(.is_sleep_prevent.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_acl_aclink_status.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_acl_find_by_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_acl_find_by_hci.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_acl_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_acl_link_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_acl_ready.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_disconnect_done.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_done_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_extended_done_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_find_by_local_cid.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_find_by_remote_cid.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_cid_update_default.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_data_le_oriented_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_data_le_oriented_recv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_data_le_oriented_send.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_data_send_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_get_signal_table.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_psm_find.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_psm_find_mask.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_psm_find_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_psm_reg_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_psm_unreg_cbk_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_security_cbk2cid.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_security_hci_req.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_set_acllink_mask_gle.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2cap_signal_le_connection_param_check.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2capc_done.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2capc_psm_reg_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.l2capc_psm_un_reg_cbk.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_connect.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_connect_white_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_ext_connect.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_link_find_on_local.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_local_set_own_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_resolving_list_add.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_set_advertise_command.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_set_advertising_parameters_core.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_set_scan_command.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_set_scan_parmeters.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_white_list_add.romcall.text)); . = ALIGN(8); KEEP(*.o*(.le_white_list_clear.romcall.text)); . = ALIGN(8); KEEP(*.o*(.leave_critical.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lec_white_list_remove.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_conn_update_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_create_desc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_enable_encryption_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_feats_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_exception_hci_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_llcp_reject_ext_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_llcp_unknown_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_init_desc_for_pwr_ctrl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_map_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_phy_upd_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_srv_data_length_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_srv_data_length_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_ver_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_clear_assigned_adv_act.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_comm_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_notify_per_adv_fsm.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_ext_initiate_set_acl_con_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_initiate_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_per_sync_mgr_fsm_get_id.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_scan_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_add_map_assessment_module.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_get_ble_chnl_cls.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_get_ble_chnl_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_get_gle_chnl_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_remove_map_assessment_module.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_check_exist_srv_fsm.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_data_length_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_disc_with_terminate.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_disc_without_terminate.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_enc_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_exit_associate_sync_link.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_fsm_create_srv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_fsm_delete_srv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_fsm_find_srv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_map_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_phy_update_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_send_basic_cmp_evt_with_hdl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_send_reject.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_acb_ver_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_gle_feats_get_acb_feat_bit.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print0.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print1.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print4.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print_alterable_para_press.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_oam_status_store.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memcmp.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memcpy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memmove.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kfree.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kmalloc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.panic.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_convert_tx_pwr_val_to_less_level.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_ble_acl_rssi_avg_cnt.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_ble_acl_rssi_avg_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_ble_tx_estimate_power_value.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_dft_tx_power_level.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_rf_rx_path_compensation_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_rf_tx_path_compensation_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_set_rf_rx_path_compensation_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_set_rf_tx_path_compensation_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.rxpd_get_rx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.rxpd_get_rx_phy_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.schedule_loop_cleanup.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sed_ble_set_rx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sed_ble_set_tx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.set_event.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_compare_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_ecdh_dh_key_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_ecdh_public_key_check_on_curve_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_ecdh_public_key_reserv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_get_l2cap_cid_by_link_type_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smpc_cleanup_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smpc_done_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smpc_gle_link_type_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smpc_init_reserve.romcall.text)); . = ALIGN(8); KEEP(*.o*(.strcmp.romcall.text)); . = ALIGN(8); KEEP(*.o*(.strlen.romcall.text)); . = ALIGN(8); KEEP(*.o*(.timer_add_lite.romcall.text)); . = ALIGN(8); KEEP(*.o*(.timer_del_free_lite.romcall.text)); . = ALIGN(8); KEEP(*.o*(.timer_del_lite.romcall.text)); . = ALIGN(8); KEEP(*.o*(.timer_restart_lite.romcall.text)); . = ALIGN(8); KEEP(*.o*(.tolower.romcall.text)); . = ALIGN(8); KEEP(*.o*(.txpd_ble_adv_set_txphy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.utf8to_wide.romcall.text)); . = ALIGN(8); KEEP(*.o*(.waitfor_singleobject.romcall.text)); . = ALIGN(8); KEEP(*.o*(.wide_to_utf8.romcall.text));_EVENEER_TEXT = .;. = ALIGN(8);
        _SVENEER_ROM_RAM_RO = .;; _EVENEER_ROM_RAM_RO = .;
        _SVENEER_ROM_FLASH_RO = .;; _EVENEER_ROM_FLASH_RO = .;
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    .rom_ram_data : {
        . = ALIGN(8);
        _SVENEER_ROM_RAM_D = .;KEEP(*.o*(.data.g_att_connection_timeout));KEEP(*.o*(.data.g_att_max_connection_interval));KEEP(*.o*(.data.g_att_min_connection_interval));KEEP(*.o*(.data.g_att_mtu_le_default));KEEP(*.o*(.data.g_att_mtu_le_max));KEEP(*.o*(.data.g_att_slave_latency));KEEP(*.o*(.data.g_buff_new_max_len));KEEP(*.o*(.data.g_config_le_privacy));KEEP(*.o*(.data.g_config_smp_lesc));KEEP(*.o*(.data.g_config_smp_mode));KEEP(*.o*(.data.g_em_ble_enc_cipher_offset));KEEP(*.o*(.data.g_em_ble_enc_plain_offset));KEEP(*.o*(.data.g_em_ble_fal_end));KEEP(*.o*(.data.g_em_ble_fal_offset));KEEP(*.o*(.data.g_em_ble_int_queue_cnt));KEEP(*.o*(.data.g_em_ble_int_queue_end));KEEP(*.o*(.data.g_em_ble_int_queue_offset));KEEP(*.o*(.data.g_em_ble_ral_end));KEEP(*.o*(.data.g_em_ble_ral_offset));KEEP(*.o*(.data.g_em_ble_rx_desc_cnt));KEEP(*.o*(.data.g_em_ble_rx_desc_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_cntl_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_cntl_size));KEEP(*.o*(.data.g_em_ble_tx_buff_data_cnt));KEEP(*.o*(.data.g_em_ble_tx_buff_data_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_data_size));KEEP(*.o*(.data.g_em_ble_txcntl_desc_cnt));KEEP(*.o*(.data.g_em_comm_rx_buff_cnt));KEEP(*.o*(.data.g_em_comm_rx_buff_offset));KEEP(*.o*(.data.g_em_comm_rx_buff_size));KEEP(*.o*(.data.g_em_comm_sed_offset));KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_cnt));KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_offset));KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_size));KEEP(*.o*(.data.g_em_comm_txpd_desc_count));KEEP(*.o*(.data.g_em_comm_txpd_desc_offset));KEEP(*.o*(.data.g_em_eeq_normal_cnt));KEEP(*.o*(.data.g_em_eeq_normal_offset));KEEP(*.o*(.data.g_em_eeq_urgent_cnt));KEEP(*.o*(.data.g_em_eeq_urgent_offset));KEEP(*.o*(.data.g_em_freq_tbl_offset));KEEP(*.o*(.data.g_em_gle_int_queue_cnt));KEEP(*.o*(.data.g_em_gle_int_queue_end));KEEP(*.o*(.data.g_em_gle_int_queue_offset));KEEP(*.o*(.data.g_em_gle_rx_desc_cnt));KEEP(*.o*(.data.g_em_gle_rx_desc_offset));KEEP(*.o*(.data.g_em_gle_tx_buff_data_cnt));KEEP(*.o*(.data.g_em_gle_tx_buff_data_offset));KEEP(*.o*(.data.g_em_gle_tx_buff_data_size));KEEP(*.o*(.data.g_em_size));KEEP(*.o*(.data.g_es_cancel_margin));KEEP(*.o*(.data.g_es_stop_latency));KEEP(*.o*(.data.g_gap_adv_duration_uint));KEEP(*.o*(.data.g_gap_adv_period_uint));KEEP(*.o*(.data.g_gap_adv_retry_max_num));KEEP(*.o*(.data.g_gap_extended_advertising_duration_ms));KEEP(*.o*(.data.g_gap_extended_scan_duration_ms));KEEP(*.o*(.data.g_gap_extended_scan_period_ms));KEEP(*.o*(.data.g_gap_group_vendor_specific));KEEP(*.o*(.data.g_gap_max_encry_keysize));KEEP(*.o*(.data.g_gap_min_encry_keysize));KEEP(*.o*(.data.g_gap_ops_set_local_br_addr));KEEP(*.o*(.data.g_gatt_min_encry_keysize));KEEP(*.o*(.data.g_gatt_properties_appearance));KEEP(*.o*(.data.g_gatt_properties_central_address_resolution));KEEP(*.o*(.data.g_gatt_properties_connection_parameters));KEEP(*.o*(.data.g_gatt_properties_device_name));KEEP(*.o*(.data.g_hci_periodic_advertising_create_sync_timeout));KEEP(*.o*(.data.g_hci_qos_realtime_flush_max));KEEP(*.o*(.data.g_hci_start_encryption_timeout));KEEP(*.o*(.data.g_l2cap_link_timeout));KEEP(*.o*(.data.g_l2cap_max_connection_interval));KEEP(*.o*(.data.g_l2cap_min_connection_interval));KEEP(*.o*(.data.g_l2cap_security_keyword));KEEP(*.o*(.data.g_max_nb_active_link));KEEP(*.o*(.data.g_smp_slave_peerkeys_max));; _EVENEER_ROM_RAM_D = .;
        *(.got)
        *(.got*)
        *(.igot)
        *(.igot*)
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    .plat_rom_data :
    {
        . = ALIGN(8);
        KEEP(SORT(*memset_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*memcpy_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*memmove_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*strcpy_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*strncpy_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*strcat_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*strncat_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*strtok_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*sprintf_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*snprintf_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*vsprintf_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*vsnprintf_s*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*secureprintoutput_a*.o*)(.data .data* .sdata .sdata*))
        KEEP(*(.data.strNullPointer));
        KEEP(*(.data.strNullString));
        KEEP(*(.data.wStrNullString));
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    .bth_rom_data :
    {
        . = ALIGN(8);
        KEEP(SORT(*libbg_common_rom*.a:sysdep*.o*)(.data .data* .sdata .sdata*)) KEEP(SORT(*libbt_host_rom*.a:)(.data .data* .sdata .sdata*)) KEEP(SORT(*libbg_common_rom*.a:)(.data .data* .sdata .sdata*))
        KEEP(*(.data.g_lemgr_ext_req_funcs));
        KEEP(*(.data.g_smp_lesc_enable));
        KEEP(*(.data.cmac_const_rb));
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    .btc_rom_data :
    {
        . = ALIGN(8);
        KEEP(SORT(*libbgtp_rom.a:)(.data .data* .sdata .sdata*))
        KEEP(*(.data.g_pmu_state_handler_tab)); KEEP(*(.data.g_min_deep_sleep_slot_time)); KEEP(*(.data.g_min_light_sleep_slot_time)); KEEP(*(.data.g_ble_isr_process_tbl)); KEEP(*(.data.g_current_hw_timer)); KEEP(*(.data.g_last_update_time)); KEEP(*(.data.g_company_identifier)); KEEP(*(.data.g_local_bdaddr)); KEEP(*(.data.g_sw_subversion)); KEEP(*(.data.g_sw_version)); KEEP(*(.data.g_dts_hci_config)); KEEP(*(.data.g_macro_cfg_flags)); KEEP(*(.data.g_btc_mem_pool_size)); KEEP(*(.data.g_btc_sem_evt)); KEEP(*(.data.g_crypto_algo_support)); KEEP(*(.data.g_encrypt_ccmctr_setup_tab)); KEEP(*(.data.g_encrypt_key_deriv_tab)); KEEP(*(.data.g_encrypt_key_exchange_tab)); KEEP(*(.data.g_es_last_finetarget_time)); KEEP(*(.data.g_es_last_update_time)); KEEP(*(.data.g_duration_max_dft)); KEEP(*(.data.g_evt_task_ble_adv_default_aux_channel)); KEEP(*(.data.g_evt_task_ble_adv_default_aux_offset)); KEEP(*(.data.g_gle_isr_process_tbl)); KEEP(*(.data.g_ble_dft_min_valid_chnl_num)); KEEP(*(.data.g_ble_dft_req_map_update_timer)); KEEP(*(.data.g_adv_cmd_class)); KEEP(*(.data.g_gle_dft_min_valid_chnl_num)); KEEP(*(.data.g_gle_dft_req_map_upate_timer)); KEEP(*(.data.g_tx_pwr_log_lut_tbl));
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    __romtext_end__ = .;
    __romtext_size__ = __romtext_end__ - __romtext_begin__;
    .plat_rom_bss (NOLOAD):
    {
        . = ALIGN(8);
        __rombss_begin__ = .;
        KEEP(SORT(*memset_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*memcpy_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*memmove_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*strcpy_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*strncpy_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*strcat_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*strncat_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*strtok_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*sprintf_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*snprintf_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*vsprintf_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*vsnprintf_s*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*secureprintoutput_a*.o*)(.bss .bss* .sbss .sbss* COMMON))
    } > ITCM
    .btc_rom_bss (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(SORT(*libbgtp_rom.a:)(.bss .bss* .sbss .sbss* COMMON))
        KEEP(*(.bss.g_pmu_cur_state)); KEEP(*(.bss.g_bg_sleep_boundary)); KEEP(*(.bss.g_bg_sleep_clk)); KEEP(*(.bss.g_bg_sleep_dur)); KEEP(*(.bss.g_bg_sleep_finecnt)); KEEP(*(.bss.g_bg_sleep_log_num)); KEEP(*(.bss.g_bgtp_extern_wakeup_flag)); KEEP(*(.bss.g_prevent_sleep_reasons)); KEEP(*(.bss.g_prevent_sleep_time)); KEEP(*(.bss.g_sleep_type)); KEEP(*(.bss.g_us_temp)); KEEP(*(.bss.g_ble_acl_connected_list)); KEEP(*(.bss.g_ble_isr_queue_head)); KEEP(*(.bss.g_ble_isr_queue_idx)); KEEP(*(.bss.g_ble_isr_queue_max_deep)); KEEP(*(.bss.g_timer_queue)); KEEP(*(.bss.g_dm_ble_desc)); KEEP(*(.bss.g_rand_addr)); KEEP(*(.bss.g_fal)); KEEP(*(.bss.g_rpa_engine_setup)); KEEP(*(.bss.g_dpc_env)); KEEP(*(.bss.g_btc_mem_pool)); KEEP(*(.bss.g_ecc_data)); KEEP(*(.bss.g_gle_rxpd_desc_env)); KEEP(*(.bss.g_rxpd_desc_env)); KEEP(*(.bss.g_sed_mgr)); KEEP(*(.bss.g_aes_queue)); KEEP(*(.bss.g_aes_state)); KEEP(*(.bss.g_ecc_queue)); KEEP(*(.bss.g_ecc_state)); KEEP(*(.bss.es_dur_params)); KEEP(*(.bss.g_es_finetarget_msk)); KEEP(*(.bss.g_evt_task_env_ble_initiate)); KEEP(*(.bss.g_evt_task_env_ble_scan)); KEEP(*(.bss.g_fsm_main)); KEEP(*(.bss.g_gle_isr_queue_head)); KEEP(*(.bss.g_gle_isr_queue_idx)); KEEP(*(.bss.g_gle_isr_queue_max_deep)); KEEP(*(.bss.g_lm_ble_adv_desc)); KEEP(*(.bss.g_lm_ae_comm_desc)); KEEP(*(.bss.g_lm_ble_initiate_desc)); KEEP(*(.bss.g_lm_ble_scan_desc)); KEEP(*(.bss.g_lm_comm_desc_env)); KEEP(*(.bss.g_rx_i_q_val));
    } > ITCM
    .bth_rom_bss (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(SORT(*libbg_common_rom*.a:sysdep*.o*)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*libbt_host_rom*.a:)(.bss .bss* .sbss .sbss* COMMON)) KEEP(SORT(*libbg_common_rom*.a:)(.bss .bss* .sbss .sbss* COMMON))
        KEEP(*(.bss.att_fsminst));
        KEEP(*(.bss.gap_fsminst));
        KEEP(*(.bss.g_adv_err_retry_num));
        KEEP(*(.bss.g_multi_adv_inst_max));
        KEEP(*(.bss.seq_count.21034));
        KEEP(*(.bss.hci_fsminst));
        KEEP(*(.bss.l2cap_fsminst));
        KEEP(*(.bss.smp_fsminst));
        KEEP(*(.bss.close_ev));
        KEEP(*(.bss.ind_ev));
        KEEP(*(.bss.ind_queue));
        KEEP(*(.bss.ind_queue_lock));
        KEEP(*(.bss.rx_done));
        KEEP(*(.bss.rx_ref));
        KEEP(*(.bss.g_localinfo_lock));
        KEEP(*(.bss.g_quit_ev));
        KEEP(*(.bss.handle_list));
        KEEP(*(.bss.handle_list_lock));
        . = ALIGN(8);
        . = 0x370;
        __rombss_end__ = .;
    } > ITCM
    __rombss_size__ = __rombss_end__ - __rombss_begin__;
    .plt_ramtext :
    {
        . = ALIGN(8);
        __ramtext_load__ = LOADADDR(.plt_ramtext);
        __ramtext_begin__ = .;
        *kernel/osal*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *i2c*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libflash*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libhal_flash*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libflash_porting*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *spi*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *sfc*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *tcxo*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *systick*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *middleware/chips/bs2x/pm*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *utils/pm*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *drivers/chips/bs2x/libclocks_porting.a:clocks_switch**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *chip_porting*.a:interrupt*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *kernel/non_os*.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libc.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libm.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libsegger.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libgcc.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *(.itcm.text)
        *libbase.a:*.o* (.text .text*)
        *libcsysdeps.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libinterrupt.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libriscv.a:*.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        KEEP(*patch*.o* (.text .text* .rodata .rodata* .srodata .srodata*))
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    .btc_ramtext :
    {
        . = ALIGN(8);
        __btc_ramtext_load__ = LOADADDR(.btc_ramtext);
        __btc_ramtext_begin__ = .;
        *libbgtp.a:evt_sched_ram**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:evt_sched_util_ram**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:evt_sched_mgr_ram**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:evt_prog_ram**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:dts_interrupt**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:bgtp_sleep_sw_ram**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        *libbgtp.a:bt_thread**.o* (.text .text* .rodata .rodata* .srodata .srodata*)
        __btc_ramtext_end__ = .;
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
    __btc_ramtext_size__ = __btc_ramtext_end__ - __btc_ramtext_begin__;
}
SECTIONS
{
    .usb_ramtext :
    {
        . = ALIGN(8);
        *(.text.ArchAtomicInc* .text.fhid_send_data* .text.dwc_otg_core_ep_complete* .text.LOS_AtomicInc* .text.LOS_SpinUnlockRestore* .text.LOS_IntLock* .text.LOS_SpinUnlockRestore* .text.LOS_SpinLockSave* .text.udc_interrupt* .text.ArchAtomicDec* .text.LOS_SpinLockSave* .text.usb_dma_cache_flush* .text.endpoint_arg_set* .text.LOS_AtomicDec* .text.hid_release_lock* .text.LOS_IntRestore* .text.check_and_add_hid_operations_count* .text.handle_normal_endpoint* .text.fhid_input_req_complete* .text.hid_write_data* .text.usbd_ep_queue* .text.dwc_otg_common_irq* .text.dwc_otg_ep_start_transfer* .text.hid_acquire_lock* .text.hid_submit_data* .text.dwc_otg_irq* .text.sub_hid_operations_count* .text.dwc_otg_pcd_irq* .text.dwc_otg_ep_req_start* .text.ArchWrite32* .text.ArchRead32* .text.usb_dma_cache_invalid* .text.DWC_MODIFY_REG32* .text.DWC_WRITE_REG32* .text.DWC_READ_REG32* .text.dwc_otg_epn_start_transfer* .text.handle_in_endpoint_intr*)
        *(.text.mouse_sensor_cb* .text.dongle_cb*)
        . = ALIGN(8);
    } > ITCM AT > FLASH_PROGRAM
}
__ramtext_end__ = .;
__ramtext_size__ = __ramtext_end__ - __ramtext_begin__;
SECTIONS
{
    .initcall.init : {
        . = ALIGN(16);
        __initcall0_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall0.init)))
        __initcall0_end = ABSOLUTE(.);
        __initcall1_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall1.init)))
        __initcall1_end = ABSOLUTE(.);
        __initcall2_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall2.init)))
        __initcall2_end = ABSOLUTE(.);
        __initcall3_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall3.init)))
        __initcall3_end = ABSOLUTE(.);
        __initcall4_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall4.init)))
        __initcall4_end = ABSOLUTE(.);
        __initcall5_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall5.init)))
        __initcall5_end = ABSOLUTE(.);
        __initcall6_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall6.init)))
        __initcall6_end = ABSOLUTE(.);
        __initcall7_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall7.init)))
        __initcall7_end = ABSOLUTE(.);
        . = ALIGN(16);
    } > FLASH_PROGRAM
    .text :
    {
        FILL(0xFF)
        . = ALIGN(8);
        __flash_text_begin_ = .;
        *(.text)
        *(.text*)
        . = ALIGN(4);
        __zinitcall_run_start = .;
        KEEP (*(.zinitcall.run*.init))
        __zinitcall_run_end = .;
        . = ALIGN(4);
        *(.flashtext)
        *(.flashtext*)
        *(bth_flash_data)
        *(.rodata)
        *(.rodata*)
        KEEP (*(.gdb_debug))
        __flash_text_end_ = .;
        . = ALIGN(8);
    } > FLASH_PROGRAM
    g_ram_begin = ORIGIN(DTCM);
    g_ram_size = LENGTH(DTCM);
    .data :
    {
        . = ALIGN(8);
        FILL(0xFF)
        __data_load__ = LOADADDR(.data);
        __data_begin__ = .;
        *(.data)
        *(.data*)
        *(.got)
        *(.got*)
        *(.igot)
        *(.igot*)
        _gp_ = . + 0x800;
        *(.sdata)
        *(.sdata*)
        . = ALIGN(8);
        __data_end__ = .;
    } > DTCM AT > FLASH_PROGRAM
    __data_size__ = __data_end__ - __data_begin__;
    .bss (NOLOAD) : ALIGN(0x80)
    {
        PROVIDE (__int_stack_start = .);
        *(.int_stack);
        . = ALIGN(0x4);
        PROVIDE (__int_stack_end = .);
        . = ALIGN(8);
        __bss_begin__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(.libc.errno)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = . ;
    } > ITCM
    __bss_size__ = __bss_end__ - __bss_begin__;
    .heap (NOLOAD):
    {
        . = ALIGN(8);
        g_intheap_begin = ABSOLUTE(.);
        g_intheap_size = (ORIGIN(DTCM) + LENGTH(DTCM)) - g_intheap_begin;
    } > DTCM
    .preserve (NOLOAD):
    {
        . = ALIGN(8);
        *(preserve)
    } > PRESERVE
    .cputrace_mem (NOLOAD):
    {
        . = ALIGN(8);
        KEEP(*(cputrace_mem))
    } > CPUTRACE_RAM
}
