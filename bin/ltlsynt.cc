// -*- coding: utf-8 -*-
// Copyright (C) 2017-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <config.h>

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "argmatch.h"

#include "common_aoutput.hh"
#include "common_finput.hh"
#include "common_setup.hh"
#include "common_sys.hh"

#include <spot/misc/bddlt.hh>
#include <spot/misc/game.hh>
#include <spot/tl/formula.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/aiger.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/split.hh>

enum
{
  OPT_ALGO = 256,
  OPT_INPUT,
  OPT_OUTPUT,
  OPT_PRINT,
  OPT_PRINT_AIGER,
  OPT_REAL,
  OPT_VERBOSE
};

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Input options:", 1 },
    { "ins", OPT_INPUT, "PROPS", 0,
      "comma-separated list of uncontrollable (a.k.a. input) atomic"
      " propositions", 0},
    { "outs", OPT_OUTPUT, "PROPS", 0,
      "comma-separated list of controllable (a.k.a. output) atomic"
      " propositions", 0},
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Fine tuning:", 10 },
    { "algo", OPT_ALGO, "qp|rec", 0,
      "choose the parity game algorithm, valid ones are rec (Zielonka's"
      " recursive algorithm, default) and qp (Calude et al.'s quasi-polynomial"
      " time algorithm)", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output options:", 20 },
    { "print-pg", OPT_PRINT, nullptr, 0,
      "print the parity game in the pgsolver format, do not solve it", 0},
    { "realizability", OPT_REAL, nullptr, 0,
      "realizability only, do not compute a winning strategy", 0},
    { "aiger", OPT_PRINT_AIGER, nullptr, 0,
      "prints the winning strategy as an AIGER circuit", 0},
    { "verbose", OPT_VERBOSE, nullptr, 0,
      "verbose mode", -1 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 },
  };

static const struct argp_child children[] =
  {
    { &finput_argp_headless, 0, nullptr, 0 },
    { &aoutput_argp, 0, nullptr, 0 },
    //{ &aoutput_o_format_argp, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, 0 },
    { nullptr, 0, nullptr, 0 }
  };

const char argp_program_doc[] = "\
Synthesize a controller from its LTL specification.\v\
Exit status:\n\
  0   if the input problem is realizable\n\
  1   if the input problem is not realizable\n\
  2   if any error has been reported";

static std::vector<std::string> input_aps;
static std::vector<std::string> output_aps;

bool opt_print_pg(false);
bool opt_real(false);
bool opt_print_aiger(false);

// FIXME rename options to choose the algorithm
enum solver
{
  QP,
  REC
};

static char const *const solver_args[] =
{
  "qp", "quasi-polynomial",
  "recursive",
  nullptr
};
static solver const solver_types[] =
{
  QP, QP,
  REC,
};
ARGMATCH_VERIFY(solver_args, solver_types);

static solver opt_solver = REC;
static bool verbose = false;

namespace
{

  // Ensures that the game is complete for player 0.
  // Also computes the owner of each state (false for player 0, i.e. env).
  // Initial state belongs to Player 0 and the game is turn-based.
  static std::vector<bool>
  complete_env(spot::twa_graph_ptr& arena)
  {
    unsigned sink_env = arena->new_state();
    unsigned sink_con = arena->new_state();

    auto um = arena->acc().unsat_mark();
    if (!um.first)
      throw std::runtime_error("game winning condition is a tautology");
    arena->new_edge(sink_con, sink_env, bddtrue, um.second);
    arena->new_edge(sink_env, sink_con, bddtrue, um.second);

    std::vector<bool> seen(arena->num_states(), false);
    std::vector<unsigned> todo({arena->get_init_state_number()});
    std::vector<bool> owner(arena->num_states(), false);
    owner[arena->get_init_state_number()] = false;
    owner[sink_env] = true;
    while (!todo.empty())
      {
        unsigned src = todo.back();
        todo.pop_back();
        seen[src] = true;
        bdd missing = bddtrue;
        for (const auto& e: arena->out(src))
          {
            if (!owner[src])
              missing -= e.cond;

            if (!seen[e.dst])
              {
                owner[e.dst] = !owner[src];
                todo.push_back(e.dst);
              }
          }
        if (!owner[src] && missing != bddfalse)
          arena->new_edge(src, sink_con, missing, um.second);
      }

    return owner;
  }

  static spot::twa_graph_ptr
  to_dpa(const spot::twa_graph_ptr& split)
  {
    // if the input automaton is deterministic, degeneralize it to be sure to
    // end up with a parity automaton
    auto dpa = spot::tgba_determinize(spot::degeneralize_tba(split));
    dpa->merge_edges();
    if (opt_print_pg)
      dpa = spot::sbacc(dpa);
    spot::colorize_parity_here(dpa, true);
    spot::change_parity_here(dpa, spot::parity_kind_max,
                             spot::parity_style_odd);
    bool max, odd;
    dpa->acc().is_parity(max, odd);
    assert(max && odd);
    assert(spot::is_deterministic(dpa));
    return dpa;
  }

  // Construct a smaller automaton, filtering out states that are not
  // accessible.  Also merge back pairs of p --(i)--> q --(o)--> r
  // transitions to p --(i&o)--> r.
  static spot::twa_graph_ptr
  strat_to_aut(const spot::parity_game& pg,
               const spot::parity_game::strategy_t& strat,
               const spot::twa_graph_ptr& dpa,
               bdd all_outputs)
  {
    auto aut = spot::make_twa_graph(dpa->get_dict());
    aut->copy_ap_of(dpa);
    std::vector<unsigned> todo{pg.get_init_state_number()};
    std::vector<int> pg2aut(pg.num_states(), -1);
    aut->set_init_state(aut->new_state());
    pg2aut[pg.get_init_state_number()] = aut->get_init_state_number();
    while (!todo.empty())
      {
        unsigned s = todo.back();
        todo.pop_back();
        for (auto& e1: dpa->out(s))
          {
            unsigned i = 0;
            for (auto& e2: dpa->out(e1.dst))
              {
                bool self_loop = false;
                if (e1.dst == s || e2.dst == e1.dst)
                  self_loop = true;
                if (self_loop || strat.at(e1.dst) == i)
                  {
                    bdd out = bdd_satoneset(e2.cond, all_outputs, bddfalse);
                    if (pg2aut[e2.dst] == -1)
                      {
                        pg2aut[e2.dst] = aut->new_state();
                        todo.push_back(e2.dst);
                      }
                    aut->new_edge(pg2aut[s], pg2aut[e2.dst],
                                  (e1.cond & out), {});
                    break;
                  }
                ++i;
              }
          }
      }
    aut->purge_dead_states();
    aut->set_named_prop("synthesis-outputs", new bdd(all_outputs));
    return aut;
  }

  class ltl_processor final : public job_processor
  {
  private:
    spot::translator& trans_;
    std::vector<std::string> input_aps_;
    std::vector<std::string> output_aps_;

  public:

    ltl_processor(spot::translator& trans,
                  std::vector<std::string> input_aps_,
                  std::vector<std::string> output_aps_)
      : trans_(trans), input_aps_(input_aps_), output_aps_(output_aps_)
    {
    }

    int process_formula(spot::formula f,
                        const char*, int) override
    {
      spot::process_timer timer;
      timer.start();

      auto aut = trans_.run(&f);
      if (verbose)
        std::cerr << "translating formula done" << std::endl;
      bdd all_inputs = bddtrue;
      bdd all_outputs = bddtrue;
      for (unsigned i = 0; i < input_aps_.size(); ++i)
        {
          std::ostringstream lowercase;
          for (char c: input_aps_[i])
            lowercase << (char)std::tolower(c);
          unsigned v = aut->register_ap(spot::formula::ap(lowercase.str()));
          all_inputs &= bdd_ithvar(v);
        }
      for (unsigned i = 0; i < output_aps_.size(); ++i)
        {
          std::ostringstream lowercase;
          for (char c: output_aps_[i])
            lowercase << (char)std::tolower(c);
          unsigned v = aut->register_ap(spot::formula::ap(lowercase.str()));
          all_outputs &= bdd_ithvar(v);
        }

      auto split = split_2step(aut, all_inputs);
      if (verbose)
        std::cerr << "split inputs and outputs done" << std::endl;
      auto dpa = to_dpa(split);
      if (verbose)
        std::cerr << "determinization done" << std::endl;
      auto owner = complete_env(dpa);
      auto pg = spot::parity_game(dpa, owner);
      if (verbose)
        std::cerr << "parity game built" << std::endl;
      timer.stop();

      if (opt_print_pg)
        {
          pg.print(std::cout);
          return 0;
        }
      switch (opt_solver)
        {
          case REC:
            {
              spot::parity_game::strategy_t strategy[2];
              spot::parity_game::region_t winning_region[2];
              pg.solve(winning_region, strategy);
              if (winning_region[1].count(pg.get_init_state_number()))
                {
                  std::cout << "REALIZABLE\n";
                  if (!opt_real)
                    {
                      auto strat_aut =
                        strat_to_aut(pg, strategy[1], dpa, all_outputs);

                      // output the winning strategy
                      if (opt_print_aiger)
                        spot::print_aiger(std::cout, strat_aut);
                      else
                        {
                          automaton_printer printer;
                          printer.print(strat_aut, timer);
                        }
                    }
                  return 0;
                }
              else
                {
                  std::cout << "UNREALIZABLE\n";
                  return 1;
                }
            }
          case QP:
            if (!opt_real)
              {
                std::cout << "The quasi-polynomial time algorithm does not"
                  " implement synthesis yet, use --realizability\n";
                return 2;
              }
            else if (pg.solve_qp())
              {
                std::cout << "REALIZABLE\n";
                return 0;
              }
            else
              {
                std::cout << "UNREALIZABLE\n";
                return 1;
              }
          default:
            SPOT_UNREACHABLE();
            return 2;
        }
    }
  };
}

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  switch (key)
    {
    case OPT_INPUT:
      {
        std::istringstream aps(arg);
        std::string ap;
        while (std::getline(aps, ap, ','))
          {
            ap.erase(remove_if(ap.begin(), ap.end(), isspace), ap.end());
            input_aps.push_back(ap);
          }
        break;
      }
    case OPT_OUTPUT:
      {
        std::istringstream aps(arg);
        std::string ap;
        while (std::getline(aps, ap, ','))
          {
            ap.erase(remove_if(ap.begin(), ap.end(), isspace), ap.end());
            output_aps.push_back(ap);
          }
        break;
      }
    case OPT_PRINT:
      opt_print_pg = true;
      break;
    case OPT_ALGO:
      opt_solver = XARGMATCH("--algo", arg, solver_args, solver_types);
      break;
    case OPT_REAL:
      opt_real = true;
      break;
    case OPT_PRINT_AIGER:
      opt_print_aiger = true;
      break;
    case OPT_VERBOSE:
      verbose = true;
      break;
    }
  return 0;
}

int
main(int argc, char **argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, nullptr,
                        argp_program_doc, children, nullptr, nullptr };
      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);
      check_no_formula();

      spot::translator trans;
      ltl_processor processor(trans, input_aps, output_aps);
      return processor.run();
    });
}
