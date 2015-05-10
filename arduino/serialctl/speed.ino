#define TERRAIN_CF_MAX 80

void r_ISR(){
        lt=micros();
        sample=1;
}

void speedreg(int forwardSpeed){
        err = (micros() - (double)lt);
        if(err > minspeed || sample){
                sample=0;
                err=err-minspeed;
                if(err < -5000){
                        cf+= 5;
                } else if(err > 10000){
                        cf-= 2;
                }
                cf = constrain(cf,-TERRAIN_CF_MAX,0);
        }
        esc.writeMicroseconds(1500 - forwardSpeed + cf);
}

