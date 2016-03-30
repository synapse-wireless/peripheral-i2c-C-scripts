#define ENABLE_BIT_DEFINITIONS
#include <ioavr.h>
#include <iom128rfa1.h>
#include <intrinsics.h>
#include "peripheral_i2c.h"

#define TWSR_STATUS_CODE                 TWSR & 0xF8

#define MT_START            0x08
#define MT_REPEAT_START     0x10
#define MT_SLA_ACK          0x18
#define MT_SLA_NACK         0x20   
#define MT_DATA_ACK         0x28
#define MT_DATA_NACK        0x30
#define MT_ARB_LOST         0x38
#define MR_SLA_ACK          0x40
#define MR_SLA_NACK         0x48
#define MR_RCV_ACKD         0x50
#define MR_RCV_NACKD        0x58


#define WAIT_FOR_TWI_INTERRUPT_CLEAR     while ( (TWCR & (1<<TWINT)) == 0 )
#define START_TRANSMISSION               TWCR = (1<<TWINT) | (1<<TWEN)

#define SET_START_CONDITION              TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA)
#define SET_STOP_CONDITION               TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO)
#define SET_RESTART_CONDITION            TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA) 

#define SEND_ACK                         TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA)
#define SEND_NACK                        TWCR = (1<<TWINT)|  (1<<TWEN)

#define NO_ERROR            0
#define ERROR_FAILED_START  -1
#define ERROR_NO_ACK        -2

uint16_t character_timeout_ticks;
uint8_t retries;

/*
*  delay
* 
*  delays 'cycles' clock ticks
*/
void delay(uint16_t cycles)
{
    TCCR1B |= (1<<CS10);
    
    TCNT1 = 0;
    while (TCNT1 < cycles)
        continue;
        
    TCCR1B &= ~(1<<CS10);
}

/*
*  i2c_init
* 
*  Sets up the I2C clock rate.  
*  Clock rate = cpu_Hz / [16 + (2*TWBR) * 4^TWPS]
*
*  Also lets you specify the timeout and how often to retry
*  checking for either the return ACK / NACK from the slave
*  device, or the incoming data when in master-receive mode.
*/
void i2c_init(uint8_t twbr, uint8_t twps, uint16_t timeout, uint8_t num_retries)
{
    twps &= 0x03;
    
    TWSR |= twps;
    TWBR = twbr;
    character_timeout_ticks = timeout;
    retries = num_retries;
    
    delay(18750);  // let it settle
}



/*
*  i2c_start
*
*  Send the start condition followed by the address of the slave device -
*  the address should already be offset and the read or write bit applied.
*
*  Note that we return int16_t because that is the return type requried for SNAPpy int.
*/
int16_t i2c_start(uint8_t address)
{
    // set interrupt flag, start flag, and enable flag
    SET_START_CONDITION;
    
    // wait for the interrupt flag - this tells us start 
    // has been transmitted
    WAIT_FOR_TWI_INTERRUPT_CLEAR;
    
    // check the TWI Status Reg and mask prescaler bits
    if ( TWSR_STATUS_CODE != MT_START )
        return ERROR_FAILED_START;
    
    // start transmission of slave address
    TWDR = 0x00ff & address;
    START_TRANSMISSION;
    
    WAIT_FOR_TWI_INTERRUPT_CLEAR;
    
    if ( TWSR_STATUS_CODE != MT_SLA_ACK && TWSR_STATUS_CODE != MR_SLA_ACK ) 
        return ERROR_NO_ACK;
        
    return NO_ERROR;
}

void i2c_stop()
{
    SET_STOP_CONDITION;
}

void i2c_restart()
{
    SET_RESTART_CONDITION;
}

/*
*  i2c_write_byte
*
*  1.  Write a single byte to the data register.
*  2.  Write the enable bits to TWCR to send it.
*  3.  Wait for confirmation it's been sent (MT_DATA_ACK in the status code)
*  4.  If we never get confirmation, even after N retries, return ERROR_NO_ACK.
*
*  Note that we return int16_t because that is the return type requried for SNAPpy int.
*/
int16_t i2c_write_byte(char __generic data)
{
    uint8_t tx_retries = retries;
    do 
    {
        TWDR = data;
        START_TRANSMISSION;
        WAIT_FOR_TWI_INTERRUPT_CLEAR;
        if ( TWSR_STATUS_CODE != MT_DATA_ACK )
        {
            // don't underflow
            if (tx_retries != 0)
                tx_retries--;
            delay(character_timeout_ticks);
            continue;
        }
        else
            return NO_ERROR;
    } while ( tx_retries > 0 );
    
    return ERROR_NO_ACK;
}

/*
*  i2c_write
*
*  Sends N bytes one at a time using i2c_write_byte
*  Sends START condition, then bytes, then either START or RESTART
*  If at some point a byte fails to send, sets STOP condition and 
*  returns error code.
*
*  Note that we return int16_t because that is the return type requried for SNAPpy int.
*/
int16_t i2c_write(uint8_t address, char __generic * data, uint8_t length, bool endWithRestart)
{
    int16_t rva = 0;
    rva = i2c_start(address);
    
    for ( int i = 0; i < length; i++ )
    {
        rva = i2c_write_byte( data[i] );
        
        if ( rva != NO_ERROR )
        {
            i2c_stop();
            return rva;
        }
    }
    
    if (endWithRestart)
        i2c_restart();
    else
        i2c_stop();
    
    return NO_ERROR;
}


/*
*  i2c_read_byte
*
*  Try to read a byte from the TWDR register.  
*  Look for either MR_RCV_ACKD or MR_RCV_NACKD 
*   - always NACK when receiving the last byte to stop the slave tx.
*  If a byte has not been received yet, delay and check again, up to retries times.
*
*  Note that we return int16_t because that is the return type requried for SNAPpy int.
*/
int16_t i2c_read_byte(char __generic *data)
{
    uint8_t twsr = 0;
    uint8_t rx_retries = retries;
    
    do 
    {
        twsr = TWSR_STATUS_CODE;
        if ( twsr == MR_RCV_ACKD || twsr == MR_RCV_NACKD ) 
        {
            *data = TWDR;
            return NO_ERROR;
        }
        else
        {
            // don't underflow
            if (rx_retries != 0)
                rx_retries--;
            delay(character_timeout_ticks);
            continue;
        }
    } while ( rx_retries > 0 );
    
    return ERROR_NO_ACK;
}


/*
*  i2c_read
*
*  NOTE:  applies the READ bit for you!
*
*  Set START condition, then read length bytes, from TWDR via i2c_read_byte.
*  Prior to reading the byte, set up to ACK so the next byte can be sent
*  or NACK if it is the last byte to end the transmission.
*  In the event i2c_read_byte fails to get a byte, set STOP and return
*  the error code.
*
*  Note that we return int16_t because that is the return type requried for SNAPpy int.
*/
int16_t i2c_read(uint8_t address, char __generic * buffer, uint8_t length, bool endWithRestart)
{
    int16_t rva = 0;
    
    rva = i2c_start(address + 1);
    if (rva != 0)
    {
        i2c_stop();
        return rva;
    }
    
    for ( int i = 0; i < length; i++ )
    {
        if ( i == length-1 )
            SEND_NACK;
        else
            SEND_ACK;
        
        rva = i2c_read_byte( buffer+i );
        
        if ( rva != NO_ERROR )
        {
            i2c_stop();
            return rva;
        }
        
        WAIT_FOR_TWI_INTERRUPT_CLEAR;
    }
    
    if (endWithRestart)
        i2c_restart();
    else
        i2c_stop();

    return NO_ERROR;   
}



