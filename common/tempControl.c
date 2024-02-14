#include "tempControl.h"
#include "constants.h"
#include "filter_sma.h"

#define NTC_UP_R 100000.0f
/* constants of Steinhart-Hart equation */
#define A 0.001111f
#define B 0.000237987f
#define C 0.000000065f

float Ntc_Tmp = 0;
uint16_t Ntc_R;

uint16_t userTemperature;
uint16_t currentTemperature;
/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
     R1(T1): 102кОм(24°С)
     R2(T2): 22кОм(90°С)
     Схема включения: A
     Ra: 10кОм
     Напряжения U0/Uref: 3.3В/3.3В
*/

const uint16_t adcDiscreteTable[] = {
    3913, 3910, 3907, 3904, 3901, 3898, 3895, 3892, 3889, 3885, 
    3882, 3879, 3876, 3872, 3869, 3865, 3862, 3858, 3855, 3851, 
    3847, 3844, 3840, 3836, 3833, 3829, 3825, 3821, 3817, 3813, 
    3809, 3805, 3801, 3797, 3793, 3788, 3784, 3780, 3776, 3771, 
    3767, 3762, 3758, 3753, 3749, 3744, 3740, 3735, 3730, 3726, 
    3721, 3716, 3711, 3706, 3701, 3696, 3691, 3686, 3681, 3676, 
    3671, 3665, 3660, 3655, 3650, 3644, 3639, 3633, 3628, 3622, 
    3617, 3611, 3605, 3600, 3594, 3588, 3583, 3577, 3571, 3565, 
    3559, 3553, 3547, 3541, 3535, 3529, 3522, 3516, 3510, 3504, 
    3497, 3491, 3485, 3478, 3472, 3465, 3459, 3452, 3446, 3439, 
    3432, 3426, 3419, 3412, 3405, 3399, 3392, 3385, 3378, 3371, 
    3364, 3357, 3350, 3343, 3336, 3329, 3321, 3314, 3307, 3300, 
    3292, 3285, 3278, 3270, 3263, 3256, 3248, 3241, 3233, 3226, 
    3218, 3211, 3203, 3195, 3188, 3180, 3172, 3165, 3157, 3149, 
    3141, 3133, 3126, 3118, 3110, 3102, 3094, 3086, 3078, 3070, 
    3062, 3054, 3046, 3038, 3030, 3022, 3014, 3006, 2998, 2990, 
    2981, 2973, 2965, 2957, 2949, 2941, 2932, 2924, 2916, 2908, 
    2899, 2891, 2883, 2874, 2866, 2858, 2849, 2841, 2833, 2824, 
    2816, 2808, 2799, 2791, 2782, 2774, 2766, 2757, 2749, 2740, 
    2732, 2724, 2715, 2707, 2698, 2690, 2681, 2673, 2665, 2656, 
    2648, 2639, 2631, 2622, 2614, 2606, 2597, 2589, 2580, 2572, 
    2564, 2555, 2547, 2538, 2530, 2522, 2513, 2505, 2496, 2488, 
    2480, 2471, 2463, 2455, 2446, 2438, 2430, 2421, 2413, 2405, 
    2396, 2388, 2380, 2372, 2363, 2355, 2347, 2339, 2331, 2322, 
    2314, 2306, 2298, 2290, 2282, 2274, 2265, 2257, 2249, 2241, 
    2233, 2225, 2217, 2209, 2201, 2193, 2185, 2177, 2169, 2162, 
    2154, 2146, 2138, 2130, 2122, 2114, 2107, 2099, 2091, 2083, 
    2076, 2068, 2060, 2053, 2045, 2037, 2030, 2022, 2015, 2007, 
    2000, 1992, 1985, 1977, 1970, 1962, 1955, 1947, 1940, 1933, 
    1925, 1918, 1911, 1903, 1896, 1889, 1882, 1875, 1867, 1860, 
    1853, 1846, 1839, 1832, 1825, 1818, 1811, 1804, 1797, 1790, 
    1783, 1776, 1770, 1763, 1756, 1749, 1742, 1736, 1729, 1722, 
    1715, 1709, 1702, 1696, 1689, 1682, 1676, 1669, 1663, 1656, 
    1650, 1644, 1637, 1631, 1624, 1618, 1612, 1605, 1599, 1593, 
    1587, 1581, 1574, 1568, 1562, 1556, 1550, 1544, 1538, 1532, 
    1526, 1520, 1514, 1508, 1502, 1496, 1490, 1485, 1479, 1473, 
    1467, 1461, 1456, 1450, 1444, 1439, 1433, 1427, 1422, 1416, 
    1411, 1405, 1400, 1394, 1389, 1383, 1378, 1373, 1367, 1362, 
    1357, 1351, 1346, 1341, 1336, 1330, 1325, 1320, 1315, 1310, 
    1305, 1300, 1295, 1290, 1285, 1280, 1275, 1270, 1265, 1260, 
    1255, 1250, 1245, 1240, 1236, 1231, 1226, 1221, 1217, 1212, 
    1207, 1203, 1198, 1193, 1189, 1184, 1180, 1175, 1170, 1166, 
    1161, 1157, 1153, 1148, 1144, 1139, 1135, 1131, 1126, 1122, 
    1118, 1113, 1109, 1105, 1101, 1097, 1092, 1088, 1084, 1080, 
    1076, 1072, 1068, 1064, 1060, 1056, 1052, 1048, 1044, 1040, 
    1036, 1032, 1028, 1024, 1020, 1017, 1013, 1009, 1005, 1001, 
    998, 994, 990, 987, 983, 979, 976, 972, 968, 965, 961, 958, 
    954, 951, 947, 943, 940, 937, 933, 930, 926, 923, 919, 916, 
    913, 909, 906, 903, 899, 896, 893, 890, 886, 883, 880, 877, 
    874, 870, 867, 864, 861, 858, 855, 852, 849, 846, 843, 840, 
    837, 834, 831, 828, 825, 822, 819, 816, 813, 810, 807, 804, 
    801, 799, 796, 793, 790, 787, 785, 782, 779, 776, 774, 771, 
    768, 766, 763, 760, 758, 755, 752, 750, 747, 744, 742, 739, 
    737, 734, 732, 729, 727, 724, 722, 719, 717, 714, 712, 709, 
    707, 705, 702, 700, 697, 695, 693, 690, 688, 686, 683, 681, 
    679, 677, 674, 672, 670, 667, 665, 663, 661, 659, 656, 654, 
    652, 650, 648, 646, 643, 641, 639, 637, 635, 633, 631, 629, 
    627, 625, 623, 621, 619, 617, 615, 613, 611
};

const int WINDOW_SIZE = 12; // Размер окна скользящего среднего

uint16_t slidingAverage(const uint16_t data[], int dataSize, int currentIndex) {
    uint32_t sum = 0;
    for (int i = currentIndex; i > currentIndex - WINDOW_SIZE; --i) {
        if (i >= 0) {
            sum += data[i];
        }
    }
    uint16_t average = sum / WINDOW_SIZE;
    return average;
}

// Функция вычисляет значение температуры в десятых долях градусов Цельсия
// в зависимости от суммарного значения АЦП.
uint16_t calcTemperature(uint16_t adcsum)
{
    temperature_table_index_type l = 0;
    temperature_table_index_type r = (sizeof(adcDiscreteTable) / sizeof(adcDiscreteTable[0])) - 1;
    temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);

    if (adcsum <= thigh)
    {
    #ifdef TEMPERATURE_UNDER
            if (adcsum < thigh)
                return TEMPERATURE_UNDER;
    #endif
            return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
        }
        temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
        if (adcsum >= tlow)
        {
    #ifdef TEMPERATURE_OVER
            if (adcsum > tlow)
                return TEMPERATURE_OVER;
    #endif
        return TEMPERATURE_TABLE_START;
    }

    // Двоичный поиск по таблице
    while ((r - l) > 1)
    {
        temperature_table_index_type m = (l + r) >> 1;
        temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
        if (adcsum > mid)
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }
    temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);

    if (adcsum >= vl) {
        return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
    }

    temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
    temperature_table_entry_type vd = vl - vr;
    uint16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP;

    if (vd) {
        // Линейная интерполяция
        res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
    }
    return res;
}

void updateTemp(volatile uint16_t *adcData) { 
    uint16_t adcSum;
    for (uint8_t i = 0; i < WINDOW_SIZE; i++) {
        adcSum = slidingAverage((const uint16_t*)adcData, 64, i);
    }
    currentTemperature = calcTemperature(adcSum);
    // Ntc_R = ((NTC_UP_R)/((4095.0/adcSum) - 1));
	// /* temp */
	// float Ntc_Ln = log(Ntc_R);
	// /* calc. temperature */
	// Ntc_Tmp = (1.0/(A + B*Ntc_Ln + C*Ntc_Ln*Ntc_Ln*Ntc_Ln)) - 273.15;
    // currentTemperature = Ntc_Tmp;
}

void updateScreen(void) {
    char str[10];
    // Display ADC value
    // snprintf(str, sizeof(str), "%.2f", currentTemperature);

    sprintf(str, "%d", currentTemperature);
    ssd1306_SetCursor(80, 15);
    ssd1306_WriteString(str, Font_6x8, White);
    // Display user temperature
    
    sprintf(str, "%d", userTemperature);
    ssd1306_SetCursor(80, 30);
    ssd1306_WriteString(str, Font_6x8, White);

    ssd1306_UpdateScreen();
}