use <MCAD/boxes.scad>
use <rcube.scad>
use <lcd.scad>
$fa=1;
$fs=0.4;
L=120;
l=77;
h=12;
thickness=1;
high_box_radius=3; //3;
low_box_radius=15; //3;
rebord_assemblage=0.20; // %

lcd_L = 42;
lcd_l = 64;




module capot_superieur(){
    difference() {
        // forme exterieur
        rcube([L, l, h],
            [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);

        translate([thickness,thickness,0])
        union() {
        // vidage interieur
        rcube([L-2*thickness, l-2*thickness, h-thickness],
            [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);

        // encoche inferieur
        translate([-thickness/2,-thickness/2,0])
        rcube([L-2*thickness/2, l-2*thickness/2, rebord_assemblage*h],
            [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);
        //emplacement clavier:
        rcube([56-2*thickness, l-2*thickness, h],
            [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);
        }
        //emplacemebt LCD
        translate([ thickness+L*7.5/10-lcd_L/2,
                    l/2-lcd_l/2,
                    0])
        rcube([lcd_L,lcd_l, h],
            [0,0,0,0]);
    }
    // keyboard
    rotate([0, 180, 0]) 
    translate([-3,-62,-h-2.5])
    import("C:/Users/cvaug/Documents/PlatformIO/Projects/Esp32Pda/3dmodel/5393306_BBQ20_Keyboard_top_cover/files/coverplate012.stl");
    
    // LCD
    #support_lcd([ thickness+L*7.5/10-lcd_L/2,
                    l/2-lcd_l/2,
                    h-thickness]); //TODO
}

module capot_inferieur(){
    // forme exterieur
    rcube([L, l, thickness],
        [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);


    translate([thickness,thickness,0]){
    // encoche inferieur
    translate([-thickness/2,-thickness/2,0])
    rcube([L-2*thickness/2, l-2*thickness/2, rebord_assemblage*h],
        [low_box_radius,high_box_radius,high_box_radius,low_box_radius]);
    }

}


capot_superieur( );

//translate([0,0,-10])
rotate([180, 0, 0])
translate([0,10,-h])
capot_inferieur( );    



//esp32s3
//color("black") {
//    translate([0,0,-100])
//    import("C:/Users/cvaug/Documents/PlatformIO/Projects/Esp32Pda/3dmodel/UnexpectedMaker/FeatherS3_P4.stl");
//}

