// #include "common.h"
// #include "weatherApp.hpp"
// #include "../../../conf.h"

// UnixTime currentTime(0);

// OpenWeatherOneCall OWOC;

// // lv_obj_t *time_label;
// // lv_obj_t *weather_label;
// // lv_obj_t *weather_icon;

// void weather_timer(/*lv_timer_t * timer*/)
// {
// //   /*Use the user_data*/
// //   uint32_t * user_data = timer->user_data;

//     OWOC.parseWeather();

//     //lv_label_set_text_fmt(weather_label, /*"%d:%d:%d\n%s - %d°"*/ "n%s - %d°", /*currentTime.hour, currentTime.minute, currentTime.second,*/
//     //  OWOC.current->summary, OWOC.current->temperature);
// }

// // void time_timer(lv_timer_t * timer)
// // {
// //     currentTime.getDateTime(ntp.getUnixTime());
    
// //     lv_label_set_text_fmt(time_label, "%02d:%02d:%02d", currentTime.hour, currentTime.minute, currentTime.second);
  
// //     if ( ! (OWOC.current == NULL || OWOC.current->summary == NULL) ) {
// //         lv_label_set_text_fmt(weather_label, "%s - %d°", OWOC.current->summary, OWOC.current->temperature);


// //         // "http://openweathermap.org/img/w/" + OWOC.current->icon + ".png"

// //     }
// // }


// void weatherSetup( /*lv_obj_t *scr , lv_indev_t* indev_keyboard */ void ) {

//   OWOC.setExcl( /*EXCL_C*/ EXCL_D+EXCL_M+EXCL_H+EXCL_A) ;
//   OWOC.setDateTimeFormat(DMY24H); 
//   OWOC.setLatLon( 48.68190406612592, 2.030943951662991) ; // Herbouvilier
//   OWOC.setUnits(METRIC);
//   //OWOC.setLanguage("fr");

//   int errorM = OWOC.setOpenWeatherKey(OPENWEATHERMAP_KEY) ;
//   if ( errorM ) {
//     printf( " Error returned: %s\ n" , OWOC.getErrorMsgs(errorM) );
//     return;
//   }


//   // // weather gui setup

//   //   // create groupe:
//   //   lv_group_t *g = lv_group_create();
//   //   lv_indev_set_group(indev_keyboard, g);

//   //   /* Create text zone */

//   //   lv_obj_t * ta;
//   //   ta = lv_textarea_create(scr);
//   //   lv_obj_align(ta, LV_ALIGN_TOP_MID, 10, 10);
//   //   //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
//   //   // lv_textarea_set_placeholder_text(ta, "Hello");
//   //   lv_obj_set_size(ta, 140*2, 80);
//   //   lv_group_add_obj(g, ta);

//   //   /* Create time label */
//   //   time_label = lv_label_create( scr );
//   //   lv_label_set_text(time_label, "Hello");
//   //   lv_obj_align( time_label, LV_ALIGN_CENTER, 0, 0 );
//   //   lv_group_add_obj(g, time_label);

//   //   /* Create weather label */
//   //   weather_label = lv_label_create( scr );
//   //   lv_label_set_text(weather_label, "-");
//   //   lv_obj_align( weather_label, LV_ALIGN_BOTTOM_MID, 0, 0 );
//   //   lv_group_add_obj(g, weather_label);

//   //   /* create weather icon */
//   //   // LV_IMG_DECLARE(img_weather_png);
//   //   // weather_icon = lv_img_create(scr);
//   //   // lv_img_set_srcnet(weather_icon, &img_weather_png);
//   //   // lv_obj_align(weather_icon, LV_ALIGN_LEFT_MID, 20, 0);
 
//   //   // static uint32_t user_data = 10;
//   //   lv_timer_t * wtimer = lv_timer_create(weather_timer, 1000*60*10,  NULL); // 10min
//   //   lv_timer_ready(wtimer);
//   //   lv_timer_t * ttimer = lv_timer_create(time_timer, 1000,  NULL); // 1s

// }


