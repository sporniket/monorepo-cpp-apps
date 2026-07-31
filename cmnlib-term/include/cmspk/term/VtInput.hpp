// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUT_HPP__
#define __CMSPK__TERM__VTINPUT_HPP__

#include <string>

#include "cmspk/io/DataSource.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================
/**
 * Representation of keyboard key (either isolated or combined with a modifier).
 *
 * * By a "happy coïncidence", the numeric value of `CTRL + letter` and some
 *   special key (`RETURN`, `HTAB`, `ESCAPE`, and `BACKSPACE`) **will** be the
 *   value of their single octet representation in Vt.
 * * By a "happy coïncidence", any sequence of up to 8 octets mapped to a key 
 *   **will** have a value representing the sequence of octets, padded with
 *   zeros, e.g. for keys that are mapped to `CSI(escape+'[') + ASCII char`,
 *   the numeric value of those keys **will** be a 24-bits value `0x1b5b**`
 *   with `**` being the ASCII code of the last character of the sequence.
 * * `VtInputKey` has `HTAB` and `RETURN` instead of `CTRL_I` and `CTRL_M`.
 */
enum class VtInputKey : uint64_t {
  /**
   * Key combination : `CTRL` + `A`.
   */
  CTRL_A=1,
  /**
   * Key combination : `CTRL` + `B`.
   */
  CTRL_B,
  /**
   * Key combination : `CTRL` + `C`.
   */
  CTRL_C,
  /**
   * Key combination : `CTRL` + `D`.
   */
  CTRL_D,
  /**
   * Key combination : `CTRL` + `E`.
   */
  CTRL_E,
  /**
   * Key combination : `CTRL` + `F`.
   */
  CTRL_F,
  /**
   * Key combination : `CTRL` + `G`.
   */
  CTRL_G,
  /**
   * Key combination : `CTRL` + `H`.
   */
  CTRL_H,
  /**
   * `Horizontal tabulation` key.
   */
  HTAB,
  /**
   * Key combination : `CTRL` + `J`.
   */
  CTRL_J,
  /**
   * Key combination : `CTRL` + `K`.
   */
  CTRL_K,
  /**
   * Key combination : `CTRL` + `L`.
   */
  CTRL_L,
  /**
   * `Carriage return` key.
   */
  RETURN,
  /**
   * Key combination : `CTRL` + `N`.
   */
  CTRL_N,
  /**
   * Key combination : `CTRL` + `O`.
   */
  CTRL_O,
  /**
   * Key combination : `CTRL` + `P`.
   */
  CTRL_P,
  /**
   * Key combination : `CTRL` + `Q`.
   */
  CTRL_Q,
  /**
   * Key combination : `CTRL` + `R`.
   */
  CTRL_R,
  /**
   * Key combination : `CTRL` + `S`.
   */
  CTRL_S,
  /**
   * Key combination : `CTRL` + `T`.
   */
  CTRL_T,
  /**
   * Key combination : `CTRL` + `U`.
   */
  CTRL_U,
  /**
   * Key combination : `CTRL` + `V`.
   */
  CTRL_V,
  /**
   * Key combination : `CTRL` + `W`.
   */
  CTRL_W,
  /**
   * Key combination : `CTRL` + `X`.
   */
  CTRL_X,
  /**
   * Key combination : `CTRL` + `Y`.
   */
  CTRL_Y,
  /**
   * Key combination : `CTRL` + `Z`.
   */
  CTRL_Z,
  /**
   * `ESCAPE` key.
   */
  ESCAPE,
  /**
   * `BACKSPACE` key.
   */
  BACKSPACE=127,
  /**
   * Arrow key : up
   */
  ARROW_UP=0x1b5b01,
  /**
   * Arrow key : down
   */
  ARROW_DOWN,
  /**
   * Arrow key : left
   */
  ARROW_LEFT,
  /**
   * Arrow key : right
   */
  ARROW_RIGHT
};

/**
 * Type of VtInputReport.
 *
 * @see VtInputReport
 */
enum class VtInputReporVtpe {
  /**
   * A report containing the cursor position, e.g "\x1b[24;80R".
   */
  CURSOR_POSITION
};

/**
 * Representation of a null octet received as character input.
 *
 * An empty type, to be returned when the datasource get zero (0), which should not happen in a terminal input.
 */
class VtInputNull {};

/**
 * Representation of a Vt report, like the report of the cursor position.
 *
 * @param CharT the character type used to store string parameters of the reports.
 */
template<class CharT>
struct BasicVtInputReport {
  VtInputReporVtpe type;
  std::vector<std::basic_string<CharT>> parameters;
};

/**
 * Representation of a Vt input : either a null octet, a key stroke/combination, a report, or a simple printable character.
 *
 * @param CharT the character type used for the report string parameters and the printable characters.
 */
template <class CharT>
using BasicVtInput = std::variant<VtInputNull, VtInputKey, BasicVtInputReport<CharT>, CharT> ;

/**
 * Representation of a Vt input to be returned by an ASCII character source.
 *
 * To be used when a multi-octets charset decoding has not happened yet. 
 */
using AsciiVtInput = BasicVtInput<char8_t>;

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif

