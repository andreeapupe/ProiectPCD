#include <string.h>

#include "car.h"
#include "ezxml.h"
#include "config/cfg.h"

ezxml_t load_child(const char *name, ezxml_t parent) {
	if (NULL != parent) {
		return ezxml_child(parent, name);
	}
	return NULL;
}

void load_child_value(const char *name, ezxml_t parent, char *oValue) {
	ezxml_t child = load_child(name, parent);
	if (NULL != child) {
		strcpy(oValue, child->txt);
		printf("parsed: %s = %s\n", name, oValue);
	}
	else {
		oValue[0] = 0;
	}
}

int parse_car(char *xmlPath, Car_t *car) {
	ezxml_t xml = ezxml_parse_file(xmlPath);

	ezxml_t uid, data, speed, datetime, time;

	if (NULL == xml) {
		printf("Cannot open xml file at %s\n", xmlPath);
		return FALSE;
	}

	printf("Parsing car from xml file at %s\n", xmlPath);

	uid = load_child("uid", xml);
	data = load_child("data", xml);
	speed = load_child("speed", data);
	datetime = load_child("datetime", data);
	time = load_child("time", datetime);

	load_child_value("model", uid, car->model);
	load_child_value("vin", uid, car->vin);

	load_child_value("value", speed, car->speed);
	load_child_value("measureUnit", speed, car->speedUnit);

	load_child_value("date", datetime, car->date);

	load_child_value("value", time, car->time);
	load_child_value("timezone", time, car->timeZone);

	load_child_value("attachment", xml, car->attachmentPath);

	ezxml_free(xml);

	return TRUE;
}