#include "protocol.hpp"

const QMap<uint8_t, StrmDataInfo> strm_data_infos = {
    { PACKET_DAT_ACC,       { "Acc",      PACKET_DAT_ACC,       { "AccX", "AccY", "AccZ" } } },
    { PACKET_DAT_ANG,       { "Ang",      PACKET_DAT_ANG,       { "AngX", "AngY", "AngZ" } } },
    { PACKET_DAT_MAG,       { "Mag",      PACKET_DAT_MAG,       { "MagX", "MagY", "MagZ" } } },
    { PACKET_DAT_RAW_ACC,   { "RawAcc",   PACKET_DAT_RAW_ACC,   { "RawAccX", "RawAccY", "RawAccZ" } } },
    { PACKET_DAT_RAW_ANG,   { "RawAng",   PACKET_DAT_RAW_ANG,   { "RawAngX", "RawAngY", "RawAngZ" } } },
    { PACKET_DAT_RAW_MAG,   { "RawMag",   PACKET_DAT_RAW_MAG,   { "RawMagX", "RawMagY", "RawMagZ" } } },
    { PACKET_DAT_KF_QUAT,   { "KFQuat",   PACKET_DAT_KF_QUAT,   { "KFQuatW", "KFQuatX", "KFQuatY", "KFQuatZ" } } },
    { PACKET_DAT_KF_RPY,    { "KFRPY",    PACKET_DAT_KF_RPY,    { "KFRoll", "KFPitch", "KFYaw" } } },
    { PACKET_DAT_MEAS_QUAT, { "MeasQuat", PACKET_DAT_MEAS_QUAT, { "MeasQuatW", "MeasQuatX", "MeasQuatY", "MeasQuatZ" } } },
    { PACKET_DAT_MEAS_RPY,  { "MeasRPY",  PACKET_DAT_MEAS_RPY,  { "MeasRoll", "MeasPitch", "MeasYaw" } } },
    { PACKET_DAT_COV_STATE, { "CovState", PACKET_DAT_COV_STATE, { "P00", "P01", "P02", "P03",
                                                                  "P10", "P11", "P12", "P13",
                                                                  "P20", "P21", "P22", "P23",
                                                                  "P30", "P31", "P32", "P33" } } },
    { PACKET_DAT_COV_PROC,  { "CovProc",  PACKET_DAT_COV_PROC,  { "Q00", "Q01", "Q02", "Q03",
                                                                  "Q10", "Q11", "Q12", "Q13",
                                                                  "Q20", "Q21", "Q22", "Q23",
                                                                  "Q30", "Q31", "Q32", "Q33" } } },
    { PACKET_DAT_COV_MEAS,  { "CovMeas",  PACKET_DAT_COV_MEAS,  { "R00", "R01", "R02", "R03", "R04", "R05",
                                                                  "R10", "R11", "R12", "R13", "R14", "R15",
                                                                  "R20", "R21", "R22", "R23", "R24", "R25",
                                                                  "R30", "R31", "R32", "R33", "R34", "R35",
                                                                  "R40", "R41", "R42", "R43", "R44", "R45",
                                                                  "R50", "R51", "R52", "R53", "R54", "R55" } } },
    { PACKET_DAT_RF_CH1,    { "RFCH1",    PACKET_DAT_RF_CH1,    { "RFCH1" } } },
    { PACKET_DAT_RF_CH2,    { "RFCH2",    PACKET_DAT_RF_CH2,    { "RFCH2" } } },
    { PACKET_DAT_RF_CH3,    { "RFCH3",    PACKET_DAT_RF_CH3,    { "RFCH3" } } },
    { PACKET_DAT_RF_CH4,    { "RFCH4",    PACKET_DAT_RF_CH4,    { "RFCH4" } } },
    { PACKET_DAT_RF_CH5,    { "RFCH5",    PACKET_DAT_RF_CH5,    { "RFCH5" } } },
    { PACKET_DAT_RF_CH6,    { "RFCH6",    PACKET_DAT_RF_CH6,    { "RFCH6" } } },
    { PACKET_DAT_RF_CH7,    { "RFCH7",    PACKET_DAT_RF_CH7,    { "RFCH7" } } },
    { PACKET_DAT_RF_CH8,    { "RFCH8",    PACKET_DAT_RF_CH8,    { "RFCH8" } } },
    { PACKET_DAT_RF_CH9,    { "RFCH9",    PACKET_DAT_RF_CH9,    { "RFCH9" } } },
    { PACKET_DAT_RF_CH10,   { "RFCH10",   PACKET_DAT_RF_CH10,   { "RFCH10" } } },
    { PACKET_DAT_RF_CH11,   { "RFCH11",   PACKET_DAT_RF_CH11,   { "RFCH11" } } },
    { PACKET_DAT_RF_CH12,   { "RFCH12",   PACKET_DAT_RF_CH12,   { "RFCH12" } } },
    { PACKET_DAT_RF_CH13,   { "RFCH13",   PACKET_DAT_RF_CH13,   { "RFCH13" } } },
    { PACKET_DAT_RF_CH14,   { "RFCH14",   PACKET_DAT_RF_CH14,   { "RFCH14" } } },
    { PACKET_DAT_RF_CH15,   { "RFCH15",   PACKET_DAT_RF_CH15,   { "RFCH15" } } },
    { PACKET_DAT_RF_CH16,   { "RFCH16",   PACKET_DAT_RF_CH16,   { "RFCH16" } } },
};
