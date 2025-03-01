#ifndef AHT_H
#define AHT_H

void initAHT(void);
int getAHT(char *temperature, char *humidity);
bool checkAHTConnection(void);
void floatToString(float value, char* buffer, int precision);

#endif // AHT_H