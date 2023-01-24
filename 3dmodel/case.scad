use <MCAD/boxes.scad>
use <lcd.scad>
$fa=1;
$fs=0.4;
L=120;
l=77;
h=12;
thickness=1;
high_box_radius=3; //3;
low_box_radius=15; //3;
rebord_assemblage=0.25; // %


module capot_superieur(){
// tour
difference() {
    union(){
        //high
        translate([L/4*2/3,0,0])
            roundedBox(size=[L*2/3,l,h],radius=high_box_radius,sidesonly=true);
        //low
        translate([-L/4*2/3,0,0])
            roundedBox(size=[L*2/3,l,h],radius=low_box_radius,sidesonly=true);
    }
    union(){
        //high
        translate([(L/4*2/3)-thickness,0,0])
            roundedBox(size=[L*2/3-thickness,l-thickness*2,h*2],radius=high_box_radius,sidesonly=true);
            // rebord inferieur pour assemblage
        translate([(L/4*2/3),0,-h/2*(1-rebord_assemblage)])
            roundedBox(size=[(L*2/3)-thickness,l-thickness/2,h*rebord_assemblage],radius=high_box_radius,sidesonly=true);
     
        //low 
        translate([-((L/4*2/3)-thickness),0,0])
            union() {
            roundedBox(size=[L*2/3-thickness,l-thickness*2,h*2],radius=low_box_radius,sidesonly=true);
            // rebord inferieur pour assemblage
            translate([-L/128,0,-h/2*(1-rebord_assemblage)])
                roundedBox(size=[(L*2/3)-thickness,l-thickness/2,h*rebord_assemblage],radius=low_box_radius,sidesonly=true);
            }       
    }
}
    
    // capot supérieur
     translate([L/4,0,+h/2]) 
    roundedBox(size=[L/2,l,thickness],radius=high_box_radius,sidesonly=true);
     translate([L/16-6,0,+h/2]) 
     cube(size=[L/8,l,thickness],center=true);

translate([0,-l/2,+h/2]) 
    #support_lcd();
     
    // keyboard
    rotate([0, 180, 0]) 
    translate([57,-100.5,-h+3.5])
    import("C:/Users/cvaug/Documents/PlatformIO/Projects/Esp32Pda/3dmodel/5393306_BBQ20_Keyboard_top_cover/files/coverplate012.stl");
}

    capot_superieur();    

//esp32s3
//color("black") {
//    translate([0,0,-100])
//    import("C:/Users/cvaug/Documents/PlatformIO/Projects/Esp32Pda/3dmodel/UnexpectedMaker/FeatherS3_P4.stl");
//}

