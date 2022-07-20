/*
  EEPROM.h - EEPROM library
  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.
  'String' put/get by Paul Stoffregen
  SAMD21 compatibility by Ben Rose for TinyCircuits 9 June 2016 and 27 May 2022 :)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if defined(ARDUINO_ARCH_SAMD)

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>

uint8_t readFlash(uint32_t readaddr);
void writeFlash(uint32_t writeaddr, uint8_t data);
void writePage(uint32_t pageAddr, uint8_t * dataToWrite);
void program_lock_region_fuse();

#define E2END 1024*16;

/***
    EERef class.
    
    This object references an EEPROM cell.
    Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
    This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.
***/

struct EERef{

    EERef( const int index )
        : index( index )                 {}
    
    //Access/read members.
    uint8_t operator*() const            { return readFlash( /*(uint8_t*)*/ index ); }
    operator const uint8_t() const       { return **this; }
    
    //Assignment/write members.
    EERef &operator=( const EERef &ref ) { return *this = *ref; }
    EERef &operator=( uint8_t in )       { return writeFlash( /*(uint8_t*)*/ index, in ), *this;  }
    EERef &operator +=( uint8_t in )     { return *this = **this + in; }
    EERef &operator -=( uint8_t in )     { return *this = **this - in; }
    EERef &operator *=( uint8_t in )     { return *this = **this * in; }
    EERef &operator /=( uint8_t in )     { return *this = **this / in; }
    EERef &operator ^=( uint8_t in )     { return *this = **this ^ in; }
    EERef &operator %=( uint8_t in )     { return *this = **this % in; }
    EERef &operator &=( uint8_t in )     { return *this = **this & in; }
    EERef &operator |=( uint8_t in )     { return *this = **this | in; }
    EERef &operator <<=( uint8_t in )    { return *this = **this << in; }
    EERef &operator >>=( uint8_t in )    { return *this = **this >> in; }
    
    EERef &update( uint8_t in )          { return  in != *this ? *this = in : *this; }
    
    /** Prefix increment/decrement **/
    EERef& operator++()                  { return *this += 1; }
    EERef& operator--()                  { return *this -= 1; }
    
    /** Postfix increment/decrement **/
    uint8_t operator++ (int){ 
        uint8_t ret = **this;
        return ++(*this), ret;
    }

    uint8_t operator-- (int){ 
        uint8_t ret = **this;
        return --(*this), ret;
    }
    
    int index; //Index of current EEPROM cell.
};

/***
    EEPtr class.
    
    This object is a bidirectional pointer to EEPROM cells represented by EERef objects.
    Just like a normal pointer type, this can be dereferenced and repositioned using 
    increment/decrement operators.
***/

struct EEPtr{

    EEPtr( const int index )
        : index( index )                {}
        
    operator const int() const          { return index; }
    EEPtr &operator=( int in )          { return index = in, *this; }
    
    //Iterator functionality.
    bool operator!=( const EEPtr &ptr ) { return index != ptr.index; }
    EERef operator*()                   { return index; }
    
    /** Prefix & Postfix increment/decrement **/
    EEPtr& operator++()                 { return ++index, *this; }
    EEPtr& operator--()                 { return --index, *this; }
    EEPtr operator++ (int)              { return index++; }
    EEPtr operator-- (int)              { return index--; }

    int index; //Index of current EEPROM cell.
};

/***
    EEPROMClass class.
    
    This object represents the entire EEPROM space.
    It wraps the functionality of EEPtr and EERef into a basic interface.
    This class is also 100% backwards compatible with earlier Arduino core releases.
***/

struct EEPROMClass{

    //Basic user access methods.
    EERef operator[]( const int idx )    { return idx; }
    uint8_t read( int idx )              { return EERef( idx ); }
    void write( int idx, uint8_t val )   { (EERef( idx )) = val; }
    void update( int idx, uint8_t val )  { EERef( idx ).update( val ); }
    
    //STL and C++11 iteration capability.
    EEPtr begin()                        { return 0x00; }
    EEPtr end()                          { return length(); } //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
    uint16_t length()                    { return E2END + 1; }
    
    //Functionality to 'get' and 'put' objects to and from EEPROM.
    template< typename T > T &get( int idx, T &t ){
        EEPtr e = idx;
        uint8_t *ptr = (uint8_t*) &t;
        for( int count = sizeof(T) ; count ; --count, ++e )  *ptr++ = *e;
        return t;
    }
    
    template< typename T > const T &put( int idx, const T &t ){
        EEPtr e = idx;
        const uint8_t *ptr = (const uint8_t*) &t;
        for( int count = sizeof(T) ; count ; --count, ++e )  (*e).update( *ptr++ );
        return t;
    }
};

	
// put - Specialization for Arduino Strings -------------------------------
// to put an Arduino String to the EEPROM we copy its internal buffer
// including the trailing \0 to the eprom
template <>
inline const String &EEPROMClass::put(int idx, const String &s)
{
    const uint8_t *ptr = (uint8_t *)s.c_str();
    EEPtr e = idx;
    for (int count = s.length() + 1; count; --count, ++e)
        (*e).update(*ptr++);
    return s;
}
// get - Specialization for Arduino Strings -------------------------------
// to "get" an Arduino String from the EEPROM we append chars from the EEPROM
// into it until we find the delimiting /0.
// String.append is not very efficient, code could probably be opitimized if required...
template <>
inline String &EEPROMClass::get(int idx, String &s){
    s = "";             // just in case...
    EEPtr e = idx;
    char c = *e;        // read in bytes until we find the terminating \0
    while (c != '\0')
    {
        s.concat(c);
        c = *(++e);
    }
    return s;
}


uint8_t readFlash(uint32_t readaddr){
  return (((uint8_t *)((256-16) * 1024 + readaddr))[0]);
}

void writeFlash(uint32_t writeaddr, uint8_t data) {
  // Check correct fuse settings
  if (*(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1) & 0x80000000)
    program_lock_region_fuse();
  
  uint32_t PAGE_SIZE = 1<<(NVMCTRL->PARAM.bit.PSZ+3);
  uint32_t ROW_SIZE = PAGE_SIZE * NVMCTRL_ROW_PAGES;
  //SerialUSB.println(PAGE_SIZE);
  //uint8_t page_buffer[PAGE_SIZE];
  //const uint32_t byte_in_page = writeaddr%PAGE_SIZE;
  //const uint32_t page_start = (256-16) * 1024 + writeaddr - byte_in_page;
  uint8_t row_buffer[ROW_SIZE];
  const uint32_t byte_in_row = writeaddr%ROW_SIZE;
  const uint32_t row_start = ((256-16) * 1024) + writeaddr - byte_in_row;

  // Read page to buffer
  uint32_t i;
  for (i = 0; i < (ROW_SIZE / 4); i++) {
    ((uint32_t *)row_buffer)[i] = ((uint32_t *)row_start)[i];
  }
  
  // Modify byte in buffer
  row_buffer[byte_in_row]=data;
  
  // Unlock region
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  NVMCTRL->ADDR.reg = ((256 - 16) * 1024) / 2;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  
  // Erase row
  NVMCTRL->ADDR.reg = row_start / 2;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
  while (NVMCTRL->INTFLAG.bit.READY == 0)
    ;
  
  // Write row
  writePage(row_start+(PAGE_SIZE*0), row_buffer+(PAGE_SIZE*0));
  writePage(row_start+(PAGE_SIZE*1), row_buffer+(PAGE_SIZE*1));
  writePage(row_start+(PAGE_SIZE*2), row_buffer+(PAGE_SIZE*2));
  writePage(row_start+(PAGE_SIZE*3), row_buffer+(PAGE_SIZE*3));
  
  // Lock Region
  NVMCTRL->ADDR.reg = ((256 - 16) * 1024) / 2;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_LR;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
}

void writePage(uint32_t pageAddr, uint8_t * dataToWrite) {
  uint32_t PAGE_SIZE = 1<<(NVMCTRL->PARAM.bit.PSZ+3);
  // Set automatic page write
  NVMCTRL->CTRLB.bit.MANW = 0;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  // Set first address
  NVMCTRL->ADDR.reg = pageAddr / 2;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
  
  // Execute "PBC" Page Buffer Clear
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
  while (NVMCTRL->INTFLAG.bit.READY == 0);

  // Fill page buffer
  uint32_t i;
  for (i = 0; i < (PAGE_SIZE / 4); i++) {
    ((uint32_t *)pageAddr)[i] = ((uint32_t *)dataToWrite)[i];
  }

  // Execute "WP" Write Page
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
  while (NVMCTRL->INTFLAG.bit.READY == 0);
}

void program_lock_region_fuse()
{
  //SerialUSB.println("Running program_lock_region_fuse()");
  uint32_t temp = 0;

  uint32_t samd21_old_fusebits[2];

  /* Make sure the module is ready */
  /* Wait for NVM command to complete */
  while (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));

  /* Read the fuse settings in the user row, 64 bit */

  samd21_old_fusebits[0] = *((uint32_t *)NVMCTRL_AUX0_ADDRESS);
  samd21_old_fusebits[1] = *(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1);

  uint32_t samd21_new_fusebits[2] = {samd21_old_fusebits[0], samd21_old_fusebits[1] & 0x7FFFFFFF}; // Lock last memory region

  /* Disable Cache */
  temp = NVMCTRL->CTRLB.reg;

  NVMCTRL->CTRLB.reg = temp | NVMCTRL_CTRLB_CACHEDIS;

  /* Clear error flags */
  NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;

  /* Set address, command will be issued elsewhere */
  NVMCTRL->ADDR.reg = NVMCTRL_AUX0_ADDRESS / 2;

  /* Erase the user page */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_EAR | NVMCTRL_CTRLA_CMDEX_KEY;

  /* Wait for NVM command to complete */
  while (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));

  /* Clear error flags */
  NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;

  /* Set address, command will be issued elsewhere */
  NVMCTRL->ADDR.reg = NVMCTRL_AUX0_ADDRESS / 2;

  /* Erase the page buffer before buffering new data */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;

  /* Wait for NVM command to complete */
  while (!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));

  /* Clear error flags */
  NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;

  /* Set address, command will be issued elsewhere */
  NVMCTRL->ADDR.reg = NVMCTRL_AUX0_ADDRESS / 2;

  *((uint32_t *)NVMCTRL_AUX0_ADDRESS) = samd21_new_fusebits[0];
  *(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1) = samd21_new_fusebits[1];

  /* Write the user page */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_WAP | NVMCTRL_CTRLA_CMDEX_KEY;

  /* Restore the settings */
  NVMCTRL->CTRLB.reg = temp;
}

static EEPROMClass EEPROM;
#endif

#endif /*defined(ARDUINO_ARCH_SAMD)*/