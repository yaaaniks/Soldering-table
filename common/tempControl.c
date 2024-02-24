#include "tempControl.h"
#include "constants.h"

#define NTC_UP_R 45000.0f
/* constants of Steinhart-Hart equation */

uint32_t resistance;

uint16_t userTemperature;
float currentTemperature;

// Значение температуры, возвращаемое если сумма результатов АЦП больше первого значения таблицы
#define TEMPERATURE_UNDER 3000
// Значение температуры, возвращаемое если сумма результатов АЦП меньше последнего значения таблицы
#define TEMPERATURE_OVER 0
// Значение температуры соответствующее первому значению таблицы
#define TEMPERATURE_TABLE_START 0
// Шаг таблицы 
#define TEMPERATURE_TABLE_STEP 5

// Тип каждого элемента в таблице, если сумма выходит в пределах 16 бит - uint16_t, иначе - uint32_t
typedef uint16_t temperature_table_entry_type;
// Тип индекса таблицы. Если в таблице больше 256 элементов, то uint16_t, иначе - uint8_t
typedef uint16_t temperature_table_index_type;
// Метод доступа к элементу таблицы, должна соответствовать temperature_table_entry_type
#define TEMPERATURE_TABLE_READ(i) termo_table[i]

/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
     R1(T1): 100кОм(25°С)
     B25/100: 3950
     Схема включения: A
     Ra: 100кОм
     Напряжения U0/Uref: 5В/3.6В
*/
const temperature_table_entry_type termo_table[] = {
4090, 4059, 4028, 3996, 3965, 3933, 3902, 3870,
    3837, 3805, 3773, 3740, 3707, 3675, 3642, 3609,
    3576, 3542, 3509, 3476, 3443, 3409, 3376, 3342,
    3309, 3275, 3242, 3209, 3175, 3142, 3108, 3075,
    3042, 3009, 2976, 2943, 2910, 2877, 2845, 2812,
    2780, 2747, 2715, 2683, 2652, 2620, 2588, 2557,
    2526, 2495, 2464, 2434, 2403, 2373, 2343, 2314,
    2284, 2255, 2226, 2197, 2169, 2140, 2112, 2085,
    2057, 2030, 2003, 1976, 1949, 1923, 1897, 1871,
    1846, 1820, 1795, 1771, 1746, 1722, 1698, 1675,
    1651, 1628, 1605, 1583, 1561, 1539, 1517, 1495,
    1474, 1453, 1433, 1412, 1392, 1372, 1352, 1333,
    1314, 1295, 1276, 1258, 1240, 1222, 1204, 1187,
    1170, 1153, 1136, 1120, 1103, 1087, 1072, 1056,
    1041, 1026, 1011, 996, 982, 967, 953, 939,
    926, 912, 899, 886, 873, 860, 848, 836,
    824, 812, 800, 788, 777, 766, 755, 744,
    733, 722, 712, 702, 692, 682, 672, 662,
    653, 644, 634, 625, 616, 608, 599, 591,
    582, 574, 566, 558, 550, 542, 535, 527,
    520, 512, 505, 498, 491, 484, 478, 471,
    464, 458, 452, 445, 439, 433, 427, 421,
    416, 410, 404, 399, 393, 388, 383, 378,
    373, 367, 363, 358, 353, 348, 343, 339,
    334, 330, 326, 321, 317, 313, 309, 305,
    301, 297, 293, 289, 285, 282, 278, 274,
    271, 267, 264, 260, 257, 254, 251, 247,
    244, 241, 238, 235, 232, 229, 226, 223,
    221, 218, 215, 213, 210, 207, 205, 202,
    200, 197, 195, 192, 190, 188, 186, 183,
    181, 179, 177, 175, 172, 170, 168, 166,
    164, 162, 161, 159, 157, 155, 153, 151,
    150, 148, 146, 144, 143, 141, 139, 138,
    136, 135, 133, 132, 130, 129, 127, 126,
    124, 123, 122, 120, 119, 117, 116, 115,
    114, 112, 111, 110, 109, 107, 106, 105,
    104, 103, 102, 101, 100, 98, 97, 96,
    95, 94, 93, 92, 91, 90, 89, 88,
    88, 87, 86, 85, 84, 83, 82, 81,
    80, 80, 79, 78, 77, 76, 76, 75,
    74, 73, 73, 72, 71, 70, 70, 69,
    68, 68, 67, 66, 66, 65, 64, 64,
    63, 62, 62, 61, 61, 60, 59, 59,
    58, 58, 57, 57, 56, 56, 55, 55,
    54, 53, 53, 52, 52, 52, 51, 51,
    50, 50, 49, 49, 48, 48, 47, 47,
    46, 46, 46, 45, 45, 44, 44, 44,
    43, 43, 42, 42, 42, 41, 41, 41,
    40, 40, 39, 39, 39, 38, 38, 38,
    37, 37, 37, 36, 36, 36, 36, 35,
    35, 35, 34, 34, 34, 33, 33, 33,
    33, 32, 32, 32, 32, 31, 31, 31,
    30, 30, 30, 30, 29, 29, 29, 29,
    29, 28, 28, 28, 28, 27, 27, 27,
    27, 27, 26, 26, 26, 26, 25, 25,
    25, 25, 25, 24, 24, 24, 24, 24,
    24, 23, 23, 23, 23, 23, 22, 22,
    22, 22, 22, 22, 21, 21, 21, 21,
    21, 21, 20, 20, 20, 20, 20, 20,
    20, 19, 19, 19, 19, 19, 19, 19,
    18, 18, 18, 18, 18, 18, 18, 18,
    17, 17, 17, 17, 17, 17, 17, 17,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13,
    12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9,
    9, 9
};

// Функция вычисляет значение температуры в десятых долях градусов Цельсия
// в зависимости от суммарного значения АЦП.
int16_t calcTemperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  
  // Проверка выхода за пределы и граничных значений
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum < thigh) 
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_OVER
      if (adcsum > tlow)
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Двоичный поиск по таблице
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Линейная интерполяция
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}

const int WINDOW_SIZE = 32; // Размер окна скользящего среднего

uint16_t slidingAverage(const uint16_t *data, int dataSize, int currentIndex) {
    uint32_t sum = 0;
    for (int i = currentIndex; i > currentIndex - WINDOW_SIZE; --i) {
        if (i >= 0) {
            sum += data[i];
        }
    }
    uint16_t average = sum / WINDOW_SIZE;
    return average;
}

void updateTemp(volatile uint16_t *adcData) { 
    uint16_t adcSum;
	uint32_t sum;
    // TODO add correct SMA filter
    for (uint8_t i = 0; i < SMP_COUNT; i++) {
        // sum += adcData[i];
		adcSum = slidingAverage((const uint16_t*)adcData, 64, i);
    }
	int16_t temp = calcTemperature(adcSum);
    currentTemperature = (float)temp / 10;
    
	// float voltage = (float)adcSum / 4095.  * 3.3;

    // currentTemperature = resistance = ((NTC_UP_R)/((4095. / (float)adcSum) - 1));
	// // /* temp */
	// float Ntc_Ln = log((float)resistance/100000.);
	// /* calc. temperature */
	// float temperature = 1 / (1 / (25.+ 273.15) + 1 / 3950. * Ntc_Ln) - 273.15;
    // currentTemperature = temperature;
    updateScreen();
}

void updateScreen(void) {
    char str[10];
    // Display ADC value
    // snprintf(str, sizeof(str), "%.2f", currentTemperature);

    sprintf(str, "%.1f", currentTemperature);
    ssd1306_FillRectangle(80, 15, 103, 30, Black);
    ssd1306_SetCursor(80, 15);
    ssd1306_WriteString(str, Font_6x8, White);
    // Display user temperature
    
    sprintf(str, "%d", userTemperature);
    // ssd1306_DrawRectangle(80, 15, 105, 30, Black);
    ssd1306_SetCursor(80, 30);
    ssd1306_WriteString(str, Font_6x8, White);

    ssd1306_UpdateScreen();
}