use <MCAD/boxes.scad>
$fa=1;
$fs=0.4;

lcd_L = 41;
lcd_l = 63;
board_L = 56;
board_l = 63;

plot_lcd_r=2.5/2;
plot_L=51;
plot_l=58.5;
board_thickness=2;
 

module plots_lcd () {
//plots
translate([0,0,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
translate([plot_L,0,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
translate([plot_L,plot_l,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
translate([0,plot_l,0])
#cylinder(h=board_thickness, r=plot_lcd_r);
}

module support_lcd(  box_thickness=1, tower_thickness = 2) {

// boarder:
difference(){
    cube(size=[board_L,
               board_l+tower_thickness*2,
               box_thickness]);
    // remove LCD:
    translate([10,board_thickness,0])
        #cube(size=[lcd_L,lcd_l,box_thickness]);
}

translate([3,2.5+tower_thickness,-board_thickness])
    plots_lcd();
}

support_lcd();