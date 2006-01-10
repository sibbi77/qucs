/***************************************************************************
                               qf_filter.h
                             ----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QF_FILTER_H
#define _QF_FILTER_H

// Header for filter

enum qf_filter_type
{
  LOWPASS,
  HIGHPASS,
  BANDPASS,
  BANDSTOP
};

typedef enum qf_filter_type qft;

enum qf_filter_kind
{
  BUTT,
  CHEB,
  ICHEB,
  BESS,
  CAUER,
  UNDEF
};

typedef enum qf_filter_kind qfk;

enum qf_ctype
{
  CAP,
  IND,
  RES
};

typedef enum qf_ctype qfct;

struct qf_comp
{
  qfct comp;
  double val;
  unsigned node1;
  unsigned node2;
};

typedef struct qf_comp qfc;

// Generic filter class

class qf_filter
{
 protected:
  const qft type;		// Lowpass, highpass…
  const qfk kind;		// Butterwth, Chebi…
  unsigned o;			// Order of filter

  const double f;		// Cutoff
  const double bw;		// Bandwidth
  const double imp;		// Terminating impedance

  // Polynomial description

  qf_poly E;			// H(ω) = E(ω)/P(ω)
  qf_poly F;			// D(ω) = F(ω)/P(ω)
  qf_poly P;

  qf_poly BN;			// B(ω) susceptance of filter
  qf_poly BD;			// B(ω) susceptance of filter

  unsigned ncomp;		// Number of components
  qfc * Comp;			// Table of components

public:
  qf_filter (void);		// Default init
  qf_filter (int);		// Init 
  qf_filter (qfk, qft);
  qf_filter (int, qfk, qft);
  qf_filter (qfk, qft, double, double, double);
  virtual ~qf_filter (void);	// Init 

  // Common routines to perform extraction of poles and zeros

  // This one extracts a finite pole of transmission
  void extract_pole_pCsLC (double, qfc *, double);

  virtual void synth (qft) = 0;	// Synthesize filter
  void to_Qucs (void);		// Outputs to Qucs
  void dump_cout (void);	// Outputs to std::cout
  void dump_spice (void);	// Outputs to std::cout
};

#endif // _QF_FILTER_H