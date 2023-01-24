use <MCAD/boxes.scad>
$fa=1;
$fs=0.4;

lcd_L = 42;
lcd_l = 64;
board_L = 56;
board_l = 64;

plot_lcd_r=2.5*0.95/2;
plot_L=51;
plot_l=58.5;
board_thickness=2;
 

module plots_lcd () {
//plots

//translate([0,0,2])
//#cylinder(h=1, r=plot_lcd_r+1);
translate([0,0,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
    
//translate([plot_L,0,2])
//#cylinder(h=1, r=plot_lcd_r+1);
translate([plot_L,0,0])
#cylinder(h=board_thickness, r=plot_lcd_r);

//translate([plot_L,plot_l,2])
//#cylinder(h=1, r=plot_lcd_r+1);
translate([plot_L,plot_l,0])
#cylinder(h=board_thickness, r=plot_lcd_r);

//translate([0,plot_l,2])
//#cylinder(h=1, r=plot_lcd_r+1);
translate([0,plot_l,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
}

module support_lcd(  box_thickness=2, tower_thickness = 2) {

// boarder:
difference(){
    cube(size=[board_L,
               board_l+tower_thickness*2,
               box_thickness]);
    // remove LCD and connectors:
    translate([9.5,board_thickness,0])
        #cube(size=[lcd_L,lcd_l,box_thickness]);
    // pin connector
    translate([board_L-5/2,(board_l+board_thickness*2)/2,box_thickness/4])
        #cube(size=[5,25,box_thickness/2], center = true);
    // nappe du lcd
    translate([7,(board_l+board_thickness*2)/2,box_thickness/4])
        #cube(size=[5,lcd_l,box_thickness/2], center = true);
    translate([6,(board_l+board_thickness*2)/2,box_thickness/4])
        #cube(size=[7,15,box_thickness/2], center = true);

}

translate([3,3+tower_thickness,-board_thickness])
    plots_lcd();
}

// support_lcd();