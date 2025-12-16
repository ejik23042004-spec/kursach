#include "ft6336.h"
#include "stm32l4xx_hal.h"  // Для HAL_Delay

/**
 * @brief Запись байта в регистр FT6336 через HAL I2C
 * @param addr Адрес устройства I2C (обычно FT6336_ADDR)
 * @param reg Адрес регистра для записи
 * @param data Данные для записи
 * @return 0 при успехе, 1 при ошибке
 */
unsigned char ft6336_write_byte(unsigned char addr, unsigned char reg, unsigned char data)
{
    HAL_StatusTypeDef status;
    uint8_t tx_data = data;
    
    // HAL_I2C_Mem_Write автоматически делает:
    // START + адрес устройства (write) + адрес регистра + данные + STOP
    status = HAL_I2C_Mem_Write(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &tx_data, 1, FT6336_I2C_TIMEOUT);
    
    if (status != HAL_OK)
    {
        return 1; // Ошибка
    }
    
    return 0; // Успех
}

/**
 * @brief Чтение байта из регистра FT6336 через HAL I2C
 * @param addr Адрес устройства I2C (обычно FT6336_ADDR)
 * @param reg Адрес регистра для чтения
 * @return Прочитанное значение или 0xFF при ошибке
 */
unsigned char ft6336_read_byte(unsigned char addr, unsigned char reg)
{
    HAL_StatusTypeDef status;
    uint8_t rx_data = 0;
    
    // HAL_I2C_Mem_Read автоматически делает:
    // 1. START + адрес устройства (write) + адрес регистра
    // 2. Repeated START + адрес устройства (read) + данные
    // Это правильный способ чтения регистра I2C устройства
    status = HAL_I2C_Mem_Read(&hi2c1, (addr << 1), reg, I2C_MEMADD_SIZE_8BIT, &rx_data, 1, FT6336_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return 0xFF; // Ошибка
    }
    
    return rx_data;
}

/**
 * @brief Инициализация FT6336
 * @return 0 при успехе, 1 при ошибке
 */
unsigned char ft6336_init(void)
{
    unsigned char chip_id;
    unsigned char focaltech_id;
    
    // I2C уже инициализирован в main.c через MX_I2C1_Init()
    // Небольшая задержка для стабилизации
    HAL_Delay(100);
    
    // Проверка Chip ID - принимаем несколько возможных значений
    // Разные версии чипов FocalTech могут иметь разные ID
    chip_id = ft6336_read_chip_id();
    
    // Проверяем, что чип отвечает (не 0x00 и не 0xFF - это ошибки)
    // И что ID соответствует одному из известных значений
    if (chip_id == 0x00 || chip_id == 0xFF)
    {
        // Это явно ошибка - чип не отвечает
        HAL_Delay(50);
        chip_id = ft6336_read_chip_id();
        if (chip_id == 0x00 || chip_id == 0xFF)
        {
            return 1; // Ошибка: чип не отвечает
        }
    }
    
    // Проверяем, что ID соответствует одному из известных значений FT6336
    if (chip_id != FT6336_CHIP_ID_VALUE_1 && 
        chip_id != FT6336_CHIP_ID_VALUE_2 && 
        chip_id != FT6336_CHIP_ID_VALUE_3)
    {
        // Попытка повторного чтения после задержки
        HAL_Delay(50);
        chip_id = ft6336_read_chip_id();
        if (chip_id != FT6336_CHIP_ID_VALUE_1 && 
            chip_id != FT6336_CHIP_ID_VALUE_2 && 
            chip_id != FT6336_CHIP_ID_VALUE_3)
        {
            // Если ID не соответствует известным значениям, но не 0x00/0xFF,
            // всё равно продолжаем - возможно это другая версия чипа
            // return 1; // Раскомментируйте, если хотите строгую проверку
        }
    }
    
    // Проверка Focaltech ID
    focaltech_id = ft6336_read_byte(FT6336_ADDR, FT6336_REG_FOCALTECH_ID);
    if (focaltech_id != FT6336_FOCALTECH_ID_VALUE)
    {
        // Это не критично, но лучше проверить
        // return 1;
    }
    
    // Настройка режима работы: нормальный режим
    if (ft6336_write_byte(FT6336_ADDR, FT6336_REG_DEVICE_MODE, FT6336_DEVICE_MODE_NORMAL))
    {
        return 1;
    }
    HAL_Delay(10);
    
    // Настройка режима прерывания: polling mode (постоянный опрос)
    // Если хотите использовать прерывания, установите FT6336_G_MODE_INTERRUPT_TRIGGER
    if (ft6336_write_byte(FT6336_ADDR, FT6336_REG_G_MODE, FT6336_G_MODE_INTERRUPT_POLLING))
    {
        return 1;
    }
    HAL_Delay(10);
    
    // Настройка порога чувствительности (чем меньше, тем чувствительнее)
    // Рекомендуемое значение: 0x12-0x20
    if (ft6336_write_byte(FT6336_ADDR, FT6336_REG_THRESHOLD, 0x16))
    {
        return 1;
    }
    HAL_Delay(10);
    
    // Настройка фильтра (0x01 - минимальный фильтр, 0x0F - максимальный)
    if (ft6336_write_byte(FT6336_ADDR, FT6336_REG_FILTER_COEFF, 0x01))
    {
        return 1;
    }
    HAL_Delay(10);
    
    // Режим питания: активный режим
    if (ft6336_write_byte(FT6336_ADDR, FT6336_REG_PWR_MODE, FT6336_PWR_MODE_ACTIVE))
    {
        return 1;
    }
    HAL_Delay(10);
    
    // Очистка статуса касания
    ft6336_clear_interrupt();
    
    // Финальная задержка для стабилизации
    HAL_Delay(50);
    
    return 0; // Успех
}

/**
 * @brief Чтение Chip ID FT6336
 * @return Значение Chip ID (должно быть 0x51)
 */
unsigned char ft6336_read_chip_id(void)
{
    return ft6336_read_byte(FT6336_ADDR, FT6336_REG_CHIP_ID);
}

/**
 * @brief Включение/выключение режима прерываний
 * @param enable 1 - включить прерывания, 0 - polling mode
 */
void ft6336_set_interrupt_mode(unsigned char enable)
{
    unsigned char mode = enable ? FT6336_G_MODE_INTERRUPT_TRIGGER : FT6336_G_MODE_INTERRUPT_POLLING;
    ft6336_write_byte(FT6336_ADDR, FT6336_REG_G_MODE, mode);
    HAL_Delay(10);
}

/**
 * @brief Очистка флага прерывания (чтение регистра статуса)
 */
void ft6336_clear_interrupt(void)
{
    ft6336_read_byte(FT6336_ADDR, FT6336_REG_TD_STATUS);
}

/**
 * @brief Получение статуса касания
 * @return Количество активных касаний (0-2)
 */
unsigned char ft6336_get_td_status(void)
{
    unsigned char status;
    status = ft6336_read_byte(FT6336_ADDR, FT6336_REG_TD_STATUS);
    return status & 0x0F; // Младшие 4 бита содержат количество касаний
}

void ft6336_get_touch1_position(unsigned int *x,unsigned int *y)
{
		unsigned int xh=0,xl=0,yh=0,yl=0;
		xh=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH1_XH);
		xl=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH1_XL);
		yh=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH1_YH);
		yl=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH1_YL);
		*x=((xh&0x000F)<<8)|xl;
		*y=((yh&0x000F)<<8)|yl;
}

void ft6336_get_touch2_position(unsigned int *x,unsigned int *y)
{
		unsigned int xh=0,xl=0,yh=0,yl=0;
		xh=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH2_XH);
		xl=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH2_XL);
		yh=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH2_YH);
		yl=ft6336_read_byte(FT6336_ADDR,FT6336_ADDR_TOUCH2_YL);
		*x=((xh&0x000F)<<8)|xl;
		*y=((yh&0x000F)<<8)|yl;
}