#ifndef _UPERF_H
#define _UPERF_H

#define PERFPOINTS_MAX 16

typedef int perfpoint_t;

struct uperf_s {
	perfpoint_t last_point;
};

perfpoint_t perfpoints[];
struct uperf_s uperf;

#endif
