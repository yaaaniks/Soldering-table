#ifndef FILTER_SMA
#define FILTER_SMA

#include <stdint.h>

// TODO: do normally this shit
/* Choose filter order */
#define FILTER_SMA_ORDER 12
/**
  * @brief Simple Moving Average (SMA) filter.
  * @note Before use define filter order.
  * @param[in] Input raw (unfiltered) value.
  * @retval Return filtered data.
  */
uint16_t Filter_SMA(uint16_t For_Filtered);

#endif // FILTER_SMA