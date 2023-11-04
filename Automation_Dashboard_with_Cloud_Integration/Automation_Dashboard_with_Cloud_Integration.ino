#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "ArduinoGraphics.h"
#include "thingProperties.h"
#include "lvgl.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield); /* Arduino_H7_Video Display(1024, 768, USBCVideo); */
Arduino_GigaDisplayTouch  TouchDetector;

lv_obj_t * meter1_label;
lv_obj_t * meter2_label;


#define MASK_WIDTH 200
#define MASK_HEIGHT 100
/**
   Draw label with gradient color (Used for giving Title in My UI)
*/

static void add_mask_event_cb(lv_event_t * e)
{
  static lv_draw_mask_map_param_t m;
  static int16_t mask_id;

  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  lv_opa_t * mask_map = (lv_opa_t*) lv_event_get_user_data(e);
  if (code == LV_EVENT_COVER_CHECK) {
    lv_event_set_cover_res(e, LV_COVER_RES_MASKED);
  }
  else if (code == LV_EVENT_DRAW_MAIN_BEGIN) {
    lv_draw_mask_map_init(&m, &obj->coords, mask_map);
    mask_id = lv_draw_mask_add(&m, NULL);

  }
  else if (code == LV_EVENT_DRAW_MAIN_END) {
    lv_draw_mask_free_param(&m);
    lv_draw_mask_remove_id(mask_id);
  }
}

void lv_title_label(void)
{
  /* Create the mask of a text by drawing it to a canvas*/
  static lv_opa_t mask_map[MASK_WIDTH * MASK_HEIGHT];

  /*Create a "8 bit alpha" canvas and clear it*/
  lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(canvas, mask_map, MASK_WIDTH, MASK_HEIGHT, LV_IMG_CF_ALPHA_8BIT);
  lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

  /*Draw a label to the canvas. The result "image" will be used as mask*/
  lv_draw_label_dsc_t label_dsc;
  lv_draw_label_dsc_init(&label_dsc);
  label_dsc.color = lv_color_white();
  label_dsc.align = LV_TEXT_ALIGN_CENTER;
  lv_canvas_draw_text(canvas, 5, 5, MASK_WIDTH, &label_dsc, "All In One\n Home Automation\n using\n Arduino IOT Cloud\n by techiesms");

  /*The mask is reads the canvas is not required anymore*/
  lv_obj_del(canvas);

  /* Create an object from where the text will be masked out.
     Now it's a rectangle with a gradient but it could be an image too*/
  lv_obj_t * grad = lv_obj_create(lv_scr_act());
  lv_obj_set_size(grad, MASK_WIDTH, MASK_HEIGHT);
  lv_obj_align(grad, LV_ALIGN_CENTER, -230, -130);
  lv_obj_set_style_bg_color(grad, lv_color_hex(0xff0000), 0);
  lv_obj_set_style_bg_grad_color(grad, lv_color_hex(0x0000ff), 0);
  lv_obj_set_style_bg_grad_dir(grad, LV_GRAD_DIR_HOR, 0);
  lv_obj_add_event_cb(grad, add_mask_event_cb, LV_EVENT_ALL, mask_map);
}


// Roller is used for Controlling Speed of Fan

/**
   Roller callback Function
*/

static void roller_event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    LV_LOG_USER("Selected value: %s", buf);

    int fan_speed_value;

    fan_speed_value = atoi(buf);

    Serial.println( fan_speed_value);
    fanSpeed.setBrightness(fan_speed_value);
  }
}

/**
   Roller with various alignments and larger text in the selected area
*/
void lv_fan_roller(void)
{
  /*A style to make the selected option larger*/
  static lv_style_t style_sel;
  lv_style_init(&style_sel);

  const char * opts = "0\n25\n50\n75\n100";
  lv_obj_t * roller;

  /*A roller on the left with left aligned text, and custom width*/
  roller = lv_roller_create(lv_scr_act());
  lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(roller, 5);
  lv_obj_set_width(roller, 180);
  lv_obj_add_style(roller, &style_sel, LV_PART_SELECTED);
  lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_align(roller, LV_ALIGN_CENTER, 230, 100);
  lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_ALL, NULL);
  lv_roller_set_selected(roller, 0, LV_ANIM_OFF);


  /*Create a style for the shadow*/
  static lv_style_t style_shadow;
  lv_style_init(&style_shadow);
  lv_style_set_text_opa(&style_shadow, LV_OPA_30);
  lv_style_set_text_color(&style_shadow, lv_color_black());

  /*Create a label for the shadow first (it's in the background)*/
  lv_obj_t * shadow_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(shadow_label, &style_shadow, 0);

  /*Create the main label*/
  lv_obj_t * main_label = lv_label_create(lv_scr_act());
  lv_label_set_text(main_label, "Fan Status");


  /*Set the same text for the shadow label*/
  lv_label_set_text(shadow_label, lv_label_get_text(main_label));

  /*Position the main label*/
  lv_obj_align(main_label, LV_ALIGN_CENTER, 230, 200);

  /*Shift the second label down and to the right by 2 pixel*/
  lv_obj_align_to(shadow_label, main_label, LV_ALIGN_TOP_LEFT, 2, 2);


}


static void btn1_event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");

  }
  else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
    Serial.println("Button1 Toggled");

    relay1 = !relay1;
  }
}

void lv_example_btn_1(void)
{

  lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn1, btn1_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, -250, 50);
  lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
  //lv_obj_set_height(btn1, LV_SIZE_CONTENT);
  lv_obj_set_size(btn1, 200, 80);

  lv_obj_t * label = lv_label_create(btn1);
  lv_label_set_text(label, "Button1");
  lv_obj_center(label);

}



static void btn2_event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
    Serial.println("Button2 Toggled");
    relay2 = !relay2;
  }
}

void lv_example_btn_2(void)
{
  lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn2, btn2_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, -250, 150);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  //lv_obj_set_height(btn1, LV_SIZE_CONTENT);
  lv_obj_set_size(btn2, 200, 80);

  lv_obj_t * label = lv_label_create(btn2);
  lv_label_set_text(label, "Button2");
  lv_obj_center(label);

}


static void btn3_event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");

  }
  else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
    Serial.println("Button3 Toggled");
    relay3 = !relay3;
  }
}

void lv_example_btn_3(void)
{
  lv_obj_t * btn3 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn3, btn3_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn3, LV_ALIGN_CENTER, -30, 50);
  lv_obj_add_flag(btn3, LV_OBJ_FLAG_CHECKABLE);
  //lv_obj_set_height(btn1, LV_SIZE_CONTENT);
  lv_obj_set_size(btn3, 200, 80);

  lv_obj_t * label = lv_label_create(btn3);
  lv_label_set_text(label, "Button3");
  lv_obj_center(label);



}



static void btn4_event_handler(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");

  }
  else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
    Serial.println("Button4 Toggled");
    relay4 = !relay4;
  }
}

void lv_example_btn_4(void)
{

  lv_obj_t * btn4 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn4, btn4_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn4, LV_ALIGN_CENTER, -30, 150);

  lv_obj_add_flag(btn4, LV_OBJ_FLAG_CHECKABLE);
  //lv_obj_set_height(btn1, LV_SIZE_CONTENT);
  lv_obj_set_size(btn4, 200, 80);

  lv_obj_t * label = lv_label_create(btn4);
  lv_label_set_text(label, "Button4");
  lv_obj_center(label);

}



static void value_changed_event_cb(lv_event_t * e);

void lv_example_arc_1(void)
{
  lv_obj_t * label = lv_label_create(lv_scr_act());

  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act());
  lv_obj_set_size(arc, 200, 200);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, 10);
  lv_obj_align(arc, LV_ALIGN_CENTER, 250, 120);
  lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

  lv_label_set_text(label, "Fan Speed");
  lv_obj_center(label);

  /*Manually update the label for the first time*/
  lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}


static lv_obj_t * meter;
static lv_meter_indicator_t * indic;

static void set_value(void * indic, int32_t v)
{
  lv_meter_set_indicator_value(meter, (lv_meter_indicator_t *) indic, v);


  lv_label_set_text_fmt(meter1_label, "Humidity - %d%%", v);

  lv_obj_align_to(meter1_label, meter, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


void lv_example_meter_1(void)
{

  meter = lv_meter_create(lv_scr_act());
  meter1_label = lv_label_create(lv_scr_act());
  //lv_obj_center(meter);
  lv_obj_align(meter, LV_ALIGN_CENTER, 250, -130);
  lv_obj_set_size(meter, 200, 200);

  /*Add a scale first*/
  lv_meter_scale_t * scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);



  /*Add a blue arc to the start*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Make the tick lines blue at the start of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                   false, 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Add a red arc to the end*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Make the tick lines red at the end of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                   0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Add a needle line indicator*/
  indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

  /*Create an animation to set the value*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_value);
  lv_anim_set_var(&a, indic);
  lv_anim_set_values(&a, 0, 100);
  lv_anim_set_time(&a, 1000);
  lv_anim_set_repeat_delay(&a, 100);
  lv_anim_set_playback_time(&a, 300);
  lv_anim_set_playback_delay(&a, 100);
  lv_anim_start(&a);


}

static lv_obj_t * meter2;
static lv_meter_indicator_t * indic2;

static void set_value2(void * indic2, int32_t v)
{
  lv_meter_set_indicator_value(meter2, (lv_meter_indicator_t *) indic2, v);


  // lv_label_set_text(lv_obj_t * meter2_label, "   ");
  lv_label_set_text_fmt(meter2_label, "Temperature - %d%", v);

  lv_obj_align_to(meter2_label, meter2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}


void lv_example_meter_2(void)
{
  meter2 = lv_meter_create(lv_scr_act());
  meter2_label = lv_label_create(lv_scr_act());
  //lv_obj_center(meter);
  lv_obj_align(meter2, LV_ALIGN_CENTER, 10, -130);
  lv_obj_set_size(meter2, 200, 200);

  /*Add a scale first*/
  lv_meter_scale_t * scale = lv_meter_add_scale(meter2);
  lv_meter_set_scale_ticks(meter2, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter2, scale, 8, 4, 15, lv_color_black(), 10);



  /*Add a blue arc to the start*/
  indic2 = lv_meter_add_arc(meter2, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_meter_set_indicator_start_value(meter2, indic2, 0);
  lv_meter_set_indicator_end_value(meter2, indic2, 20);

  /*Make the tick lines blue at the start of the scale*/
  indic2 = lv_meter_add_scale_lines(meter2, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                    false, 0);
  lv_meter_set_indicator_start_value(meter2, indic2, 0);
  lv_meter_set_indicator_end_value(meter2, indic2, 20);

  /*Add a red arc to the end*/
  indic2 = lv_meter_add_arc(meter2, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
  lv_meter_set_indicator_start_value(meter2, indic2, 80);
  lv_meter_set_indicator_end_value(meter2, indic2, 100);

  /*Make the tick lines red at the end of the scale*/
  indic2 = lv_meter_add_scale_lines(meter2, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                    0);
  lv_meter_set_indicator_start_value(meter2, indic2, 80);
  lv_meter_set_indicator_end_value(meter2, indic2, 100);

  /*Add a needle line indicator*/
  indic2 = lv_meter_add_needle_line(meter2, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

  /*Create an animation to set the value*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_value2);
  lv_anim_set_var(&a, indic2);
  lv_anim_set_values(&a, 0, 100);
  lv_anim_set_time(&a, 1000);
  lv_anim_set_repeat_delay(&a, 100);
  lv_anim_set_playback_time(&a, 300);
  lv_anim_set_playback_delay(&a, 100);
  //lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_start(&a);

}


void lv_example_img_1(void)
{

  LV_IMG_DECLARE(img_techiesmslogo);
  lv_obj_t * img1 = lv_img_create(lv_scr_act());
  lv_img_set_src(img1, &img_techiesmslogo);
  lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(img1, 200, 150);

}


void setup() {
  Display.begin();
  TouchDetector.begin();

  Serial.begin(115200);


  lv_obj_t * screen = lv_obj_create(lv_scr_act());
  lv_obj_set_size(screen, Display.width(), Display.height());

  static lv_coord_t col_dsc[] = { 500, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = { 400, LV_GRID_TEMPLATE_LAST};

  lv_obj_t * grid = lv_obj_create(lv_scr_act());

  lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

  lv_obj_set_size(grid, Display.width(), Display.height());

  lv_obj_center(grid);



  lv_example_btn_1();
  lv_example_btn_2();
  lv_example_btn_3();
  lv_example_btn_4();
  lv_example_meter_1();
  lv_example_meter_2();
  lv_fan_roller();
  lv_title_label();


  delay(2000);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();




}

void loop() {
  ArduinoCloud.update();
  lv_timer_handler();
}


void onTemperatureChange()
{
  int sensor_int_temp = temperature;
  Serial.print("Temperature-"); Serial.println(sensor_int_temp);
  set_value2(indic2, sensor_int_temp);

  // Add your code here to act upon Temperature change
}

/*
  Since Humidity is READ_WRITE variable, onHumidityChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onHumidityChange()
{
  int sensor_int_hum = humidity;
  Serial.print("Humidity-"); Serial.println(sensor_int_hum);
  set_value(indic, sensor_int_hum);
  // Add your code here to act upon Humidity change
}

/*
  Since Relay1 is READ_WRITE variable, onRelay1Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onRelay1Change()  {
  // Add your code here to act upon Relay1 change
}

/*
  Since Relay2 is READ_WRITE variable, onRelay2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onRelay2Change()  {
  // Add your code here to act upon Relay2 change
}

/*
  Since Relay3 is READ_WRITE variable, onRelay3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onRelay3Change()  {
  // Add your code here to act upon Relay3 change
}

/*
  Since Relay4 is READ_WRITE variable, onRelay4Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onRelay4Change()  {
  // Add your code here to act upon Relay4 change
}

void onFanSpeedChange()  {
  // Add your code here to act upon FanSpeed change
}
