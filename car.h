typedef struct _Car_t {
	char model[128];
	char vin[64];
	char speed[16];
	char speedUnit[32];
	char date[16];
	char time[16];
	char timeZone[16];
	char attachmentPath[256];
}Car_t;

int parse_car(char *xmlPath, Car_t *car);