var group__wicedbt__a2dp =
[
    [ "wiced_bt_a2dp_codec_info_list_t", "structwiced__bt__a2dp__codec__info__list__t.html", [
      [ "count", "structwiced__bt__a2dp__codec__info__list__t.html#a20302e2c99a60d3f612dba57e3f6333b", null ],
      [ "info", "structwiced__bt__a2dp__codec__info__list__t.html#a15ad38a53196e37cdd2036bac0a4bff6", null ]
    ] ],
    [ "wiced_bt_a2dp_config_data_t", "structwiced__bt__a2dp__config__data__t.html", [
      [ "codec_capabilities", "structwiced__bt__a2dp__config__data__t.html#a835a5142d3486d8f8b3dd6aaa1882a1d", null ],
      [ "feature_mask", "structwiced__bt__a2dp__config__data__t.html#a7baa5f2f3ee37a58dff2f9732aa14410", null ],
      [ "p_param", "structwiced__bt__a2dp__config__data__t.html#abc63638364ad26bd4e8bd13c71cd8223", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_audio_data_t", "structwiced__bt__a2dp__sink__audio__data__t.html", [
      [ "m_pt", "structwiced__bt__a2dp__sink__audio__data__t.html#a8e8371dbef14d8c53785c0033fb72687", null ],
      [ "p_pkt", "structwiced__bt__a2dp__sink__audio__data__t.html#aebb16fbf3ca243bb9e07b9b03630b243", null ],
      [ "seq_num", "structwiced__bt__a2dp__sink__audio__data__t.html#a98d07458cfe1557cbc2a1b1b2310b428", null ],
      [ "timestamp", "structwiced__bt__a2dp__sink__audio__data__t.html#ab20b0c7772544cf5d318507f34231fbe", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_status_t", "structwiced__bt__a2dp__sink__status__t.html", [
      [ "bd_addr", "structwiced__bt__a2dp__sink__status__t.html#a32720070bf9bf7f271fcb08f9354f4da", null ],
      [ "handle", "structwiced__bt__a2dp__sink__status__t.html#af242d6e3b6108ea75e1eb236e94c8240", null ],
      [ "result", "structwiced__bt__a2dp__sink__status__t.html#aaee9578b774434bb1970bf77a548f0cb", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_start_t", "structwiced__bt__a2dp__sink__start__t.html", [
      [ "handle", "structwiced__bt__a2dp__sink__start__t.html#af242d6e3b6108ea75e1eb236e94c8240", null ],
      [ "label", "structwiced__bt__a2dp__sink__start__t.html#a6abd170e74558ffbdb4fdfb852dbfe0c", null ],
      [ "result", "structwiced__bt__a2dp__sink__start__t.html#aaee9578b774434bb1970bf77a548f0cb", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_codec_config_t", "structwiced__bt__a2dp__sink__codec__config__t.html", [
      [ "codec", "structwiced__bt__a2dp__sink__codec__config__t.html#a199dfcedd0fc028b7548052db2d3b049", null ],
      [ "handle", "structwiced__bt__a2dp__sink__codec__config__t.html#af242d6e3b6108ea75e1eb236e94c8240", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_event_data_t", "unionwiced__bt__a2dp__sink__event__data__t.html", [
      [ "codec_config", "unionwiced__bt__a2dp__sink__event__data__t.html#abf70649ff72618bbc008da881aa52229", null ],
      [ "connect", "unionwiced__bt__a2dp__sink__event__data__t.html#a9c3555fc549d370822fc70fc3d873efd", null ],
      [ "disconnect", "unionwiced__bt__a2dp__sink__event__data__t.html#a7c7c1d407116fec2c5d098583ad3d9c1", null ],
      [ "start_cfm", "unionwiced__bt__a2dp__sink__event__data__t.html#a60902ba8c5fa090151be707faa139f7a", null ],
      [ "start_ind", "unionwiced__bt__a2dp__sink__event__data__t.html#a55cf87bdee780601a8a756e30352fc36", null ],
      [ "suspend", "unionwiced__bt__a2dp__sink__event__data__t.html#a5e51dacc24e7f8ff231b08ab9224d891", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_control_cb_t", "group__wicedbt__a2dp.html#gad55c1bb7ada1a4b0aa92ba8ee9d81423", null ],
    [ "wiced_bt_a2dp_sink_data_cb_t", "group__wicedbt__a2dp.html#ga756023d78d3ef6de9ead196587251523", null ],
    [ "wiced_bt_a2dp_sink_event_t", "group__wicedbt__a2dp.html#ga37267e6370fe394c7b4b73b784839c25", [
      [ "WICED_BT_A2DP_SINK_CONNECT_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25a727ce023169c312b73c5f0e9ed68b9bc", null ],
      [ "WICED_BT_A2DP_SINK_DISCONNECT_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25a95dc4f142653580512f3c5f1f8b566df", null ],
      [ "WICED_BT_A2DP_SINK_START_IND_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25a326a44e218c71139f090bf7e64edafb3", null ],
      [ "WICED_BT_A2DP_SINK_START_CFM_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25ae53eff4e80ef52bd625b01c5b602b531", null ],
      [ "WICED_BT_A2DP_SINK_SUSPEND_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25aad4eb2336a7b88d26716cdca516ec9cf", null ],
      [ "WICED_BT_A2DP_SINK_CODEC_CONFIG_EVT", "group__wicedbt__a2dp.html#gga37267e6370fe394c7b4b73b784839c25a58e21ec023504dd9d6f02dfc36eac3d8", null ]
    ] ],
    [ "wiced_bt_a2dp_sink_feature_mask_t", "group__wicedbt__a2dp.html#gaca8a4ead269e36a1c58c15c32d558628", [
      [ "WICED_BT_A2DP_SINK_FEAT_PROTECT", "group__wicedbt__a2dp.html#ggaca8a4ead269e36a1c58c15c32d558628a2b67317a7bb2a1f094df52121ace5e51", null ],
      [ "WICED_BT_A2DP_SINK_FEAT_DELAY_RPT", "group__wicedbt__a2dp.html#ggaca8a4ead269e36a1c58c15c32d558628a5ff7aaa3a220a04dbb98a75a1eb9639c", null ]
    ] ],
    [ "wiced_bt_a2d_bld_sbc_info", "group__wicedbt__a2dp.html#ga3ac42153512bea435afc160a5f38e834", null ],
    [ "wiced_bt_a2d_bld_sbc_mpl_hdr", "group__wicedbt__a2dp.html#gaa67f965fce30c36fea4d87552aaa170b", null ],
    [ "wiced_bt_a2d_pars_sbc_info", "group__wicedbt__a2dp.html#ga612847aeb5600eb2f506293ab10a20f3", null ],
    [ "wiced_bt_a2d_pars_sbc_mpl_hdr", "group__wicedbt__a2dp.html#gaa6c278e58119cb58c4e2572bb12fa42d", null ],
    [ "wiced_bt_a2d_sbc_chk_fr_init", "group__wicedbt__a2dp.html#ga2f7be66871476b08d1072024e3aac784", null ],
    [ "wiced_bt_a2d_sbc_descramble", "group__wicedbt__a2dp.html#ga97c294adaf1a468c69bf1a232c035468", null ],
    [ "wiced_bt_a2dp_sink_connect", "group__wicedbt__a2dp.html#gabd9b838fdf2bc172cadf53279c10c3b5", null ],
    [ "wiced_bt_a2dp_sink_deinit", "group__wicedbt__a2dp.html#gae517c0643780262dcf401fdeb4585b57", null ],
    [ "wiced_bt_a2dp_sink_disconnect", "group__wicedbt__a2dp.html#ga0828c47a1dd8cb9780755839b0a75ccb", null ],
    [ "wiced_bt_a2dp_sink_init", "group__wicedbt__a2dp.html#gad0f5eef5828fe9b2641507b6e8852df9", null ],
    [ "wiced_bt_a2dp_sink_lrac_switch_get", "group__wicedbt__a2dp.html#ga922a31f344d5fcbbd6bcae8cc5e177f6", null ],
    [ "wiced_bt_a2dp_sink_lrac_switch_set", "group__wicedbt__a2dp.html#ga53363f0be0ad8a9b4dd0ca74aeaca40f", null ],
    [ "wiced_bt_a2dp_sink_mute_audio", "group__wicedbt__a2dp.html#ga88234861ec64c0b7e1b7c9d6667a3ca8", null ],
    [ "wiced_bt_a2dp_sink_register_data_cback", "group__wicedbt__a2dp.html#gadb6c1854f777550c13d6841aa78a58a0", null ],
    [ "wiced_bt_a2dp_sink_send_delay_report", "group__wicedbt__a2dp.html#gad318b0a7b30513fb454ba2c2a8362e20", null ],
    [ "wiced_bt_a2dp_sink_send_start_response", "group__wicedbt__a2dp.html#gae2314335724d3a2cfc0b41da442eff1c", null ],
    [ "wiced_bt_a2dp_sink_start", "group__wicedbt__a2dp.html#ga5c3a9ef12c30d3ab765a6f2b2608bae9", null ],
    [ "wiced_bt_a2dp_sink_suspend", "group__wicedbt__a2dp.html#ga332a99805237dbd32105c9c59659312f", null ],
    [ "wiced_bt_a2dp_sink_update_route_config", "group__wicedbt__a2dp.html#gae42006edc579a4b5a4d4f91bad807a76", null ]
];