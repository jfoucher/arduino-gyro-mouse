module empty_fillet_box(size, fillet, wall_thickness) {
    fi = max(fillet - wall_thickness, 0);

    sx = size[0] + wall_thickness*2;
    sy = size[1] + wall_thickness*2;
    sz = size[2] + wall_thickness*2;
    
    difference() {
        fillet_box([sx, sy, sz], fillet);
//        scale([size[0]/sx, size[1]/sy, size[2]/sz]) {
//            translate([wall_thickness, wall_thickness, wall_thickness])
//            fillet_box([sx, sy, sz], fillets);
//        }
        
        
        translate([wall_thickness, wall_thickness, wall_thickness])
        fillet_box([size[0], size[1], size[2]], fi);
    }
    
    //echo("sx: ", sx, " sy: ", sy, " sz: ", sz);
    
    
}

module plug(h) {
    f= 0.5;
    l = 8;
    w = 20;

    translate([length/2 + wall + fillet - 4 - f, -10, wall+1])
    fillet_box([l, w, h], f);
    //cube([8, 20, 3]);
}


    

module fillet_box(size, fillet) {
    l = size[0];
    w = size[1];
    h = size[2];
    
    
    if (fillet > 0) {
        hull() {
            translate([fillet, fillet,fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([l - fillet, fillet,fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([fillet, w-fillet,fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([fillet, fillet,h-fillet]) {
                sphere(fillet, $fn=32);
            }
            
            translate([fillet, w-fillet,h-fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([l-fillet, w-fillet,fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([l-fillet, fillet,h-fillet]) {
                sphere(fillet, $fn=32);
            }
            translate([l-fillet, w-fillet, h-fillet]) {
                sphere(fillet, $fn=32);
            }
        }
    } else {
        cube(size);
    }
}

fillet = 1;
wall = 2;
step=1;

width = 36;
length = 20;
height = 7;

module bx(offset) {
    translate([step, step, 0]) {
        color("#FF0000")
            fillet_box([length + step*2 + (fillet) + offset, width + step*2 + (fillet) + offset, 20], fillet-step);
    }
}



module top() {
    difference() {
        empty_fillet_box([length + fillet, width + fillet, height + fillet], fillet, wall);
        union(){
            
            translate([-10, 10, height+fillet+1.25]){
                fillet_box([50, 20, 2], 0.5);
            }

            cube([100, 100, 3]);
        translate([0, 0, -5]) {
            difference() {
                
                    bx(0.1);
                    
                    translate([-10,-10,10]) {
                        cube([100, 100, 10]);
                    }    
                }
            }

            //translate([0, 0, -1])
            plug(3);
            
            
            translate([3.3, 21, height+4.8]){
                linear_extrude(5) {
                    text("RIGHT", 4.1);
                }
            }
            translate([3.3, 15, height+4.8]){
                linear_extrude(5) {
                    text("DROITE", 3.3);
                }
            }
        }
    }
}



module bottom() {
    
    
    difference() {
        empty_fillet_box([length + fillet, width + fillet, height + fillet], fillet, wall);
        union(){
            translate([-10, -10, 5]) {
                cube([100, 100, 20]);
            }
            
            translate([-10, 10, wall/2]){
                fillet_box([50, 20, 2.1], 0.5);
            }
            
            translate([0, 0, -2]) {
                difference() {
                    translate([-10, -10, 5]) {
                        cube([100, 100, 10]);
                    }
                    
                     bx(0);
                    
                }
            }
            color("#00FF00") {
                translate([19, 21, -4.8]){
                    linear_extrude(5) {

                        mirror([0, 1, 0])
                        rotate([0, 0, 180])
                        text("LEFT", 4.1);
                    }
                }
                translate([19, 15, -4.8]){
                    linear_extrude(5) {

                        mirror([0, 1, 0])
                        rotate([0, 0, 180])
                        text("GAUCHE", 2.3);
                    }
                }
            }
            
            
                plug(4);
        }
    }
}

//translate([0, 0, 6])
top();
//bottom();
//cube([25, 40, 10]);