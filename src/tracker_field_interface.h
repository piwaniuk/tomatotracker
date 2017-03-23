#ifndef TRACKER_FIELD_INTERFACE_H
#define TRACKER_FIELD_INTERFACE_H

#ifndef __cplusplus
#error "This header file is C++ only"
#endif

extern "C" {
#include "iterator.h"
}

// definition of the interface
struct TrackerField {
  /**
   * Constructor base
   * 
   * @param repr callback to convert a value to a C string
   */
  TrackerField(char* (*repr)(void*));
  
  /**
   * Value iterator source
   */
  virtual BidirectionalIterator* getIterator() = 0;
  
  /**
   * Current field value getter
   */
  virtual void* getValue() = 0;
  
  /**
   * Current field value setter
   */
  virtual void setValue(void* newValue) = 0;
  
  /**
   * Recently used value getter
   */
  virtual void* getRecentValue() = 0;
  
  /**
   * Recently used value setter
   */
  virtual void setRecentValue(void* newValue) = 0;
  
  /**
   * Check if a name is already used for some value
   */
  virtual bool isNameTaken(const char* name) = 0;
  
  /**
   * Launch and editor for current value
   */
  virtual void editor() = 0;
  
  /**
   * Create a new value with a given name
   */
  virtual void* createNew(const char* name) = 0;
  
  /**
   * Destroy this tracker field
   */
  virtual void destroy() = 0;
  
  /**
   * String representation callback
   */
  char* (*repr)(void*);
};

#endif
