/*

Copyright (C) 1996, 1997 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#if defined (__GNUG__) && defined (USE_PRAGMA_INTERFACE_IMPLEMENTATION)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "error.h"
#include "str-vec.h"

#include "oct-map.h"
#include "utils.h"

octave_value_list
Octave_map::operator [] (const std::string& key) const
{
  Pix p = map.seek (key);

  return p ? map.contents (p) : octave_value_list ();
}

string_vector
Octave_map::keys (void) const
{
  int len = length ();

  string_vector names (len);

  int i = 0;
  for (Pix p = first (); p != 0; next (p))
    names[i++] = key (p);

  return names;
}

int
Octave_map::array_length (void) const
{
  if (array_len == 0 && length () != 0)
    {
      Pix p = first ();
      array_len = contents(p).length ();
    }

  return array_len;
}

static string_vector
equiv_keys (const Octave_map& a, const Octave_map& b)
{
  string_vector retval;

  string_vector a_keys = a.keys().qsort ();
  string_vector b_keys = b.keys().qsort ();

  int a_len = a_keys.length ();
  int b_len = b_keys.length ();

  if (a_len == b_len)
    {
      for (int i = 0; i < a_len; i++)
	{
	  if (a_keys[i] != b_keys[i])
	    return retval;
	}

      retval = a_keys;
    }
  
  return retval;
}

Octave_map&
Octave_map::assign (const idx_vector& idx, const Octave_map& rhs)
{
  string_vector t_keys = empty () ? rhs.keys () : equiv_keys (*this, rhs);

  if (! t_keys.empty ())
    {
      int len = t_keys.length ();

      for (int i = 0; i < len; i++)
	{
	  std::string key = t_keys[i];

	  octave_value_list t_rhs = rhs[key];

	  assign (idx, key, t_rhs);

	  if (error_state)
	    break;
	}
    }
  else
    error ("field name mismatch in structure assignment");

  return *this;
}

Octave_map&
Octave_map::assign (const idx_vector& idx, const std::string& key,
		    const octave_value_list& rhs)
{
  octave_value_list tmp = map[key];

  tmp.assign (idx, rhs);

  if (! error_state)
    {
      int rhs_len = tmp.length ();

      int len = array_length ();

      octave_value fill_value = Matrix ();

      if (rhs_len < len)
	{
	  tmp.resize (len, fill_value);
	}
      else if (rhs_len > len)
	{
	  for (Pix p = first (); p != 0; next (p))
	    contents(p).resize (rhs_len, fill_value);

	  array_len = rhs_len;
	}

      map[key] = tmp;
    }

  return *this;
}

Octave_map&
Octave_map::assign (const std::string& key, const octave_value_list& rhs)
{
  if (map.empty ())
    map[key] = rhs;
  else
    {
      octave_value_list tmp = map.contents (map.first ());

      if (tmp.length () == rhs.length ())
	map[key] = rhs;
      else
	error ("invalid structure assignment");
    }

  return *this;
}

Octave_map
Octave_map::index (idx_vector& idx)
{
  Octave_map retval;

  for (Pix p = first (); p != 0; next (p))
    {
      octave_value_list tmp = contents(p).index (idx);

      if (error_state)
	break;

      retval[key(p)] = tmp;
    }

  return error_state ? Octave_map () : retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
