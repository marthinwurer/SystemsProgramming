#ifndef _KERN_BUILDINFO_H
#define _KERN_BUILDINFO_H



#ifndef BUILDINFO_BRANCH
	// git branch name that the image was built from
	#define BUILDINFO_BRANCH ""
#endif

#ifndef BUILDINFO_COMMIT
	// git commit id that the image was built from
	#define BUILDINFO_COMMIT ""
#endif

#ifndef BUILDINFO_BUILDNUM
	// number of times the image has been built
	#define BUILDINFO_BUILDNUM ""
#endif

#define BUILDINFO_STRING (BUILDINFO_BRANCH "@" BUILDINFO_COMMIT " Build: " BUILDINFO_BUILDNUM)


#endif