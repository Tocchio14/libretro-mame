/*
 * fmtowns_dsk.h
 *
 *  Created on: 23/03/2014
 */

#ifndef FMTOWNS_DSK_H_
#define FMTOWNS_DSK_H_

#include "wd177x_dsk.h"

class fmtowns_format : public wd177x_format {
public:
	fmtowns_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_FMTOWNS_FORMAT;


#endif /* FMTOWNS_DSK_H_ */
