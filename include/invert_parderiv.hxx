/************************************************************************
 * Inversion of parallel derivatives
 * Intended for use in preconditioner for reduced MHD
 * 
 * Inverts a matrix of the form 
 *
 * A + B * Grad2_par2 + C*D2DYDZ + D*D2DZ2 + E*DDY
 * 
 **************************************************************************
 * Copyright 2010 B.D.Dudson, S.Farley, M.V.Umansky, X.Q.Xu
 *
 * Contact: Ben Dudson, bd512@york.ac.uk
 * 
 * This file is part of BOUT++.
 *
 * BOUT++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BOUT++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BOUT++.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#ifndef __INV_PAR_H__
#define __INV_PAR_H__

#include "field3d.hxx"
#include "field2d.hxx"
#include "options.hxx"
#include "unused.hxx"

// Parderiv implementations
#define PARDERIVCYCLIC "cyclic"

/// Base class for parallel inversion solvers
/*!
 * 
 * Inverts a matrix of the form 
 *
 * A + B * Grad2_par2 + C*D2DYDZ + D*D2DZ2 + E*DDY
 *
 * Example
 * -------
 *
 * InvertPar *inv = InvertPar::Create();
 * inv->setCoefA(1.0);
 * inv->setCoefB(-0.1);
 * 
 * Field3D result = inv->solve(rhs);
 */
class InvertPar {
public:
  
  /*!
   * Constructor. Note that this is a base class,
   * with pure virtual members, so can't be created directly.
   * To create an InvertPar object call the create() static function.
   */ 
  InvertPar(Options *UNUSED(opt), CELL_LOC location_in, Mesh *mesh_in = nullptr)
    : location(location_in),
      localmesh(mesh_in==nullptr ? bout::globals::mesh : mesh_in) {}
  virtual ~InvertPar() = default;

  /*!
   * Create an instance of InvertPar
   * 
   * Note: For consistency this should be renamed "create" and take an Options* argument
   */
  static InvertPar* Create(Mesh *mesh_in = nullptr);
  
  /*!
   * Solve the system of equations
   * Warning: Default implementation very inefficient. This converts
   * the Field2D to a Field3D then calls solve() on the 3D variable
   */ 
  virtual const Field2D solve(const Field2D &f);
  
  /*!
   * Solve the system of equations
   *
   * This method must be implemented
   */
  virtual const Field3D solve(const Field3D &f) = 0;
  
  /*!
   * Solve, given an initial guess for the solution
   * This can help if using an iterative scheme
   */
  virtual const Field3D solve(const Field2D &f, const Field2D &UNUSED(start)) {return solve(f);}
  virtual const Field3D solve(const Field3D &f, const Field3D &UNUSED(start)) {return solve(f);}
  
  /*!
   * Set the constant coefficient A
   */
  virtual void setCoefA(const Field2D &f) = 0;
  virtual void setCoefA(const Field3D &f) {setCoefA(DC(f));}
  virtual void setCoefA(BoutReal f) {
    auto A = Field2D(f, localmesh);
    A.setLocation(location);
    setCoefA(A);
  }
  
  /*!
   * Set the Grad2_par2 coefficient B
   */ 
  virtual void setCoefB(const Field2D &f) = 0;
  virtual void setCoefB(const Field3D &f) {setCoefB(DC(f));}
  virtual void setCoefB(BoutReal f) {
    auto B = Field2D(f, localmesh);
    B.setLocation(location);
    setCoefB(B);
  }
  
  /*!
   * Set the D2DYDZ coefficient C
   */
  virtual void setCoefC(const Field2D& f) = 0;
  virtual void setCoefC(const Field3D& f) { setCoefC(DC(f)); }
  virtual void setCoefC(BoutReal f) {
    auto C = Field2D(f, localmesh);
    C.setLocation(location);
    setCoefC(C);
  }

  /*!
   * Set the D2DZ2 coefficient D
   */
  virtual void setCoefD(const Field2D& f) = 0;
  virtual void setCoefD(const Field3D& f) { setCoefD(DC(f)); }
  virtual void setCoefD(BoutReal f) {
    auto D = Field2D(f, localmesh);
    D.setLocation(location);
    setCoefD(D);
  }

  /*!
   * Set the DDY coefficient E
   */
  virtual void setCoefE(const Field2D& f) = 0;
  virtual void setCoefE(const Field3D& f) { setCoefE(DC(f)); }
  virtual void setCoefE(BoutReal f) {
    auto E = Field2D(f, localmesh);
    E.setLocation(location);
    setCoefE(E);
  }

protected:
  CELL_LOC location;
  Mesh* localmesh; ///< Mesh object for this solver

private:
};

class ParDerivFactory {
 public:
  /// Return a pointer to the only instance
  static ParDerivFactory* getInstance();

  InvertPar* createInvertPar(CELL_LOC location = CELL_CENTRE,
                             Mesh* mesh_in = bout::globals::mesh);
  InvertPar *createInvertPar(const char *type, Options *opt = nullptr,
                             CELL_LOC location = CELL_CENTRE,
                             Mesh* mesh_in = bout::globals::mesh);
  InvertPar* createInvertPar(Options *opts, CELL_LOC location = CELL_CENTRE,
                             Mesh* mesh_in = bout::globals::mesh);
 private:
  ParDerivFactory() {} // Prevent instantiation of this class
  static ParDerivFactory* instance; ///< The only instance of this class (Singleton)
};

#endif // __INV_PAR_H__

