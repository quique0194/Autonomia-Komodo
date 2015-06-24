#ifndef PID_H
#define PID_H

class PID {
public:
    PID (double _kp, double _kd, double _ki = 0) {
        kp = _kp;
        kd = _kd;
        ki = _ki;
        prev_e = 0;
    }

    double calculate (double e) {
        double p = kp*e;
        double d = ki*(e - prev_e);
        prev_e = e;
        return p + d;
    }

private:
    double kp, kd, ki;
    double prev_e;
};

#endif
