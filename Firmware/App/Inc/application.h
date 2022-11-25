#ifndef APPLICATION_H
#define APPLICATION_H

extern float acc[3], gyro[3], mag[3], pres, temp, raw_acc_meas[3], raw_gyro_meas[3], raw_mag_meas[3];
extern float kf_rpy[3], kf_acc[3], kf_vel[3], kf_pos[3], acc_rpy[3], baro_height;

void setup(void);
void loop(void);

#endif
