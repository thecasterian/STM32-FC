#ifndef APPLICATION_H
#define APPLICATION_H

extern float acc_meas[3], gyro_meas[3], mag_meas[3], baro_meas, raw_acc_meas[3], raw_gyro_meas[3], raw_mag_meas[3];
extern float kf_rpy[3], kf_acc[3], kf_vel[3], kf_pos[3], acc_rpy[3], baro_height;

void setup(void);
void loop(void);

int _write(int file, char *ptr, int len);

#endif
