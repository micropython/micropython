#define CONFIG_EXTERN_TEST      0
#define CONFIG_EXTERN_HW        0
#define CONFIG_EXTERN_CLOUD     0
#define CONFIG_TTCP		0

/* External Function */
#if CONFIG_EXTERN_TEST
extern void cmd_tcpecho(int argc, char **argv);
#endif
#if CONFIG_EXTERN_HW
extern void cmd_led(int argc, char **argv);
extern void cmd_tmp75(int argc, char **argv);
#endif
#if CONFIG_EXTERN_CLOUD
extern void cmd_cloud(int argc, char **argv);
extern void cmd_reboot(int argc, char **argv);
extern void cmd_config(int argc, char **argv);
#endif

#if CONFIG_TTCP
extern void cmd_ttcp(int argc, char **argv);
#endif

static const cmd_entry ext_cmd_table[] = {
#if CONFIG_EXTERN_TEST
	{"tcpecho", cmd_tcpecho},
#endif
#if CONFIG_EXTERN_HW
	{"led", cmd_led},
	{"tmp75", cmd_tmp75},
#endif
#if CONFIG_EXTERN_CLOUD
	{"cloud", cmd_cloud},
	{"reboot", cmd_reboot},
	{"config", cmd_config},
#endif
#if CONFIG_TTCP
	{"ttcp", cmd_ttcp},
#endif	
	{"", NULL}
};
