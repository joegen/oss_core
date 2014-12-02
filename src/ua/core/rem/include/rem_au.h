/**
 * @file rem_au.h Basic audio types
 *
 * Copyright (C) 2010 Creytiv.com
 */


/** Audio formats */
enum aufmt {
	AUFMT_S16LE,  /**< Signed 16-bit PCM */
	AUFMT_PCMA,   /**< G.711 A-law       */
	AUFMT_PCMU,   /**< G.711 U-law       */
};
