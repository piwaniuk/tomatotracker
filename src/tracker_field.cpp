extern "C" {
#include "iterator.h"
#include "widgets.h"
}

#include "tracker_field.h"
#include "tracker_field_interface.h"

static const size_t TRACKER_FIELD_WIDTH = 6;

TrackerField::TrackerField(char* (*repr)(void*)) : repr(repr) {};

static void tracker_field_choose(TrackerField& field) {
  BidirectionalIterator* iter = field.getIterator();
  void* value = field.getValue();
  void* recent = field.getRecentValue();
  void* choice;
  
  if (value != nullptr)
    iter_find_forward(iter, value);
  else if (recent != nullptr)
    iter_find_forward(iter, recent);
    
  choice = list_choice_widget(iter, TRACKER_FIELD_WIDTH, field.repr);
  iter_destroy(iter);
  
  if (choice != nullptr) {
    field.setValue(choice);
    field.setRecentValue(choice);
  }
}

static void tracker_field_reuse_recent(TrackerField& field) {
  void* recent = field.getRecentValue();
  if (recent != nullptr)
    field.setValue(recent);
}

static void tracker_field_clone(TrackerField& field) {
  //TODO: implement cloning
}

static void tracker_field_new(TrackerField& field) {
  char name[7] = "";
  ScreenPos widgetPos = get_screen_pos();
  bool hasName;
  bool editing = true;
  
  status_message("Enter a new name");
  while (editing) {
    hasName = slug_edit_widget(widgetPos, name, 6);
    if (hasName) {
      if (field.isNameTaken(name))
        status_message("Name already in use");
      else
        editing = false;
    }
    else
      editing = false;
  }
  if (hasName) {
    void* value = field.createNew(name);
    field.setValue(value);
    field.setRecentValue(value);
    status_message("Created");
  }
}

static void tracker_field_clear(TrackerField& field) {
  field.setValue(nullptr);
}

bool tracker_field_commands(TrackerField* field, int ch) {
  switch (ch) {
    case 'e': 
      field->editor();
      break;
    case '\n':
      tracker_field_choose(*field);
      break;
    case ' ':
      tracker_field_reuse_recent(*field);
      break;
    case 'c':
      tracker_field_clone(*field);
      break;
    case 'n':
      tracker_field_new(*field);
      break;
    case '.':
      tracker_field_clear(*field);
      break;
    default:
      return false;
  }
  return true;
}

void tracker_field_destroy(TrackerField* field) {
  field->destroy();
}


