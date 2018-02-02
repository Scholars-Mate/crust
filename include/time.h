/*
 * Copyright © 2018 Samuel Holland <samuel@sholland.org>
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 */

#ifndef TIME_H
#define TIME_H

#define SECONDS(n) (1000 * MSECS(n))
#define MSECS(n)   (1000 * USECS(n))
#define USECS(n)   (n)

#endif /* TIME_H */
