#include "linfit.h"
#include <float.h>
#include <math.h>
bool linfit(const float *v, const float *t, size_t n, cal_params_t *out)
{
    if (!out) return false;
    *out = (cal_params_t){0};
    if (!v || !t || n < 2 || n > UINT16_MAX) return false;
    double mean_v = 0, mean_t = 0;
    for (size_t i = 0; i < n; ++i) {
        if (!isfinite(v[i]) || !isfinite(t[i])) return false;
        mean_v += v[i]; mean_t += t[i];
    }
    mean_v /= n; mean_t /= n;
    double vv = 0, tt = 0, vt = 0;
    for (size_t i = 0; i < n; ++i) {
        double dv = v[i]-mean_v, dt = t[i]-mean_t;
        vv += dv*dv; tt += dt*dt; vt += dv*dt;
    }
    if (vv <= DBL_EPSILON || tt <= DBL_EPSILON) return false;
    double m = vt/vv, t0 = mean_t-m*mean_v, r2 = vt*vt/(vv*tt);
    if (!isfinite(m) || !isfinite(t0) || !isfinite(r2) ||
        fabs(m) > FLT_MAX || fabs(t0) > FLT_MAX) return false;
    if (r2 > 1) r2 = 1;
    out->m = (float)m; out->t0 = (float)t0; out->r2 = (float)r2;
    out->n = (uint16_t)n; out->valid = true;
    return true;
}
float cal_tj(const cal_params_t *cal, float vf)
{
    return cal && cal->valid && isfinite(vf) ? cal->m*vf+cal->t0 : NAN;
}
