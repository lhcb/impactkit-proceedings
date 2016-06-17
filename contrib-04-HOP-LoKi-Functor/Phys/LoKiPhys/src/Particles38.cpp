// $Id: Particles38.cpp 183398 2015-02-02 16:34:33Z ibelyaev $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles38.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from "M_corr" family by Mike Williams
 *
 *  This file is a part of
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement
 *   with the smear campaign of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  Last modification $Date: 2015-02-02 17:34:33 +0100 (Mon, 02 Feb 2015) $
 *                 by $Author: ibelyaev $
 *  Version           $Revision: 183398 $
 *
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-10-23
 */
// ============================================================================
/// anonymos namespace to hide local varibales
namespace
{
  // ==========================================================================
  /// the invalid 3-momentum
  const LoKi::ThreeVector s_VECTOR =  LoKi::ThreeVector ( 0 , 0 , -1 * Gaudi::Units::TeV   ) ;
  /// the invalid 3Dpoint
  const LoKi::Point3D     s_POINT  =  LoKi::Point3D     ( 0 , 0 , -1 * Gaudi::Units::km    ) ;
  // ==========================================================================
} //                                                  end of anonymos namespace
// ============================================================================
/*  constructor from the primary vertex
 *  @param x the x-position of primary vertex
 *  @param y the x-position of primary vertex
 *  @param z the x-position of primary vertex
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight
( const double x ,
  const double y ,
  const double z )
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR )
  , LoKi::Vertices::VertexHolder   ( LoKi::Point3D ( x , y , z )  )
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param pv  the primary vertex
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight
( const LHCb::VertexBase* pv    )
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR )
  , LoKi::Vertices::VertexHolder   ( pv )
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param point  the position of primary vertex
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight
( const LoKi::Point3D& point )
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR )
  , LoKi::Vertices::VertexHolder ( point )
{}
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Particles::PtFlight::~PtFlight(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PtFlight*
LoKi::Particles::PtFlight::clone() const
{ return new LoKi::Particles::PtFlight ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::PtFlight::result_type
LoKi::Particles::PtFlight::operator()
  ( LoKi::Particles::PtFlight::argument p ) const
{
  if ( 0 == p )
    {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //
  return ptFlight ( p  -> momentum() ,
                    vx -> position() , position () ) ;

}
// ============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::PtFlight::fillStream ( std::ostream& s ) const
{
  return s << "PTFLIGHT("
           << position().X() << ","
           << position().Y() << ","
           << position().Z() << ")" ;
}
// ============================================================================


// ============================================================================
/*  constructor from the primary vertex
 *  @param x the x-position of primary vertex
 *  @param y the x-position of primary vertex
 *  @param z the x-position of primary vertex
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const double x ,
  const double y ,
  const double z )
  : LoKi::Particles::PtFlight ( x , y , z )
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param pv  the primary vertex
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const LHCb::VertexBase* pv    )
  : LoKi::Particles::PtFlight ( pv )
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param point  the position of primary vertex
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const LoKi::Point3D& point )
  : LoKi::Particles::PtFlight ( point )
{}
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Particles::MCorrected::~MCorrected(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MCorrected*
LoKi::Particles::MCorrected::clone() const
{ return new LoKi::Particles::MCorrected ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::MCorrected::result_type
LoKi::Particles::MCorrected::operator()
  ( LoKi::Particles::MCorrected::argument p ) const
{
  if ( 0 == p )
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //
  return mCorrFlight ( p  -> momentum() ,
                       vx -> position() , position () ) ;

}
//============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::MCorrected::fillStream ( std::ostream& s ) const
{
  return s << "CORRM("
           << position().X() << ","
           << position().Y() << ","
           << position().Z() << ")" ;
}
// ============================================================================



// ============================================================================
// constructor
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::PtFlightWithBestVertex()
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::PtFlight ( s_POINT )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::~PtFlightWithBestVertex(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex*
LoKi::Particles::PtFlightWithBestVertex::clone() const
{ return new LoKi::Particles::PtFlightWithBestVertex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::result_type
LoKi::Particles::PtFlightWithBestVertex::operator()
  ( LoKi::Particles::PtFlightWithBestVertex::argument p ) const
{
  if ( 0 == p )
  {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv )
  {
    Error("BestVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  setVertex ( pv ) ;
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //
  return ptFlight ( p  -> momentum() ,
                    vx -> position() , position () ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::PtFlightWithBestVertex::fillStream ( std::ostream& s ) const
{ return s << "BPVPTFLIGHT" ; }
// ============================================================================





// ============================================================================
// constructor
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::MCorrectedWithBestVertex()
  : LoKi::Particles::PtFlightWithBestVertex()
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::~MCorrectedWithBestVertex(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex*
LoKi::Particles::MCorrectedWithBestVertex::clone() const
{ return new LoKi::Particles::MCorrectedWithBestVertex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::result_type
LoKi::Particles::MCorrectedWithBestVertex::operator()
  ( LoKi::Particles::MCorrectedWithBestVertex::argument p ) const
{
  if ( 0 == p )
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv )
  {
    Error("BestVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  setVertex ( pv ) ;
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //
  return mCorrFlight ( p  -> momentum() ,
                       vx -> position() , position () ) ;
}

// ============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::MCorrectedWithBestVertex::fillStream ( std::ostream& s ) const
{ return s << "BPVCORRM" ; }
// ============================================================================

// ========== //00oo..oo00// ===============
//Implementation of the Bremshtralung corrected B mass

// ============================================================================
/*  constructor from the primary vertex
 *  @param x the x-position of primary vertex
 *  @param y the x-position of primary vertex
 *  @param z the x-position of primary vertex
 */
// ============================================================================
LoKi::Particles::BremMCorrected::BremMCorrected
  ( const double x , 
    const double y , 
    const double z )
  : LoKi::Particles::PtFlight ( x , y , z)
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param pv  the primary vertex
 */
// ============================================================================
LoKi::Particles::BremMCorrected::BremMCorrected
( const LHCb::VertexBase* pv    )
  : LoKi::Particles::PtFlight ( pv )
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param point  the position of primary vertex
 */
// ============================================================================
LoKi::Particles::BremMCorrected::BremMCorrected
( const LoKi::Point3D& point )
  : LoKi::Particles::PtFlight ( point )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::BremMCorrected::~BremMCorrected(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::BremMCorrected*
LoKi::Particles::BremMCorrected::clone() const
{ return new LoKi::Particles::BremMCorrected ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::BremMCorrected::result_type
LoKi::Particles::BremMCorrected::operator()
  ( LoKi::Particles::BremMCorrected::argument p ) const
{
  if ( 0 == p )
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //

  //fill m_electrons, m_electron_mothers and m_others respectively with electrons, partickes which have only electronic daughters and all other particles
  e_finder(p->clone());
  e_finder_other(p->clone());

  LorentzVector P_h_tot, P_e_tot;
  LorentzVector P_e_corr_tot, P_e_corr_temp;

  for (const auto child : (*m_others) ) {
    P_h_tot += (child->momentum());
  }

  for (const auto child : (*m_electron_mothers) ) {
    P_e_tot += (child->momentum());
  }

  for (const auto child : (*m_rest_electrons) ) {
    P_e_tot += (child->momentum());
  }

  double pt_h = ptFlight(P_h_tot, p->endVertex()->position(), position());
  double pt_e = ptFlight(P_e_tot, p->endVertex()->position(), position());

  double alpha = pt_h/pt_e;

  for (const auto child : (*m_all_electrons) ) {
    double E_e = sqrt(pow((alpha * child->momentum().X()), 2) + pow((alpha * child->momentum().Y()), 2) + pow((alpha * child->momentum().Z()), 2) + (m_e_PDG*m_e_PDG));
    P_e_corr_temp.SetXYZT (alpha * child->momentum().X(), alpha * child->momentum().Y(), alpha * child->momentum().Z(), E_e);
    P_e_corr_tot += P_e_corr_temp;
  }

  LorentzVector P = P_h_tot + P_e_corr_tot;

  double corr_mass = P.M();

  // Cleanup
  m_rest_electrons->clear() ;
  m_all_electrons->clear() ;
  m_electron_mothers->clear() ;
  m_others->clear() ;

  return corr_mass ;
}

bool LoKi::Particles::BremMCorrected::has_only_electrons (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (child->particleID().abspid() != 11) return false ;
  }
  return true ;
}
bool LoKi::Particles::BremMCorrected::has_electron (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (!child->isBasicParticle()) {
      if (has_electron(child)) return true ;
    }
    else if (child->particleID().abspid() == 11) return true ;
  }
  return false ;
}
void LoKi::Particles::BremMCorrected::e_finder (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (child->isBasicParticle()) {
      if (child->particleID().abspid() == 11) m_all_electrons->push_back(child->clone());
      else m_others->push_back(child->clone());
    }
    else {
      if (has_electron(child)) e_finder(child);
      else m_others->push_back(child->clone());
    }
    if (!child->isBasicParticle() && has_only_electrons(child)) m_electron_mothers->push_back(child->clone());
  }
}
void LoKi::Particles::BremMCorrected::e_finder_other (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (!child->isBasicParticle() && has_only_electrons(child)) continue;
    if (child->isBasicParticle()) {
      if (child->particleID().abspid() == 11) m_rest_electrons->push_back(child->clone());
    }
    else if (has_electron(child)) e_finder_other(child);
  }
}
// ============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::BremMCorrected::fillStream ( std::ostream& s ) const
{ 
  return s << "HOPM("
           << position().X() << "," 
           << position().Y() << "," 
           << position().Z() << ")" ; 
}
// ============================================================================


// ============================================================================
// constructor
// ============================================================================
LoKi::Particles::BremMCorrectedWithBestVertex::BremMCorrectedWithBestVertex()
  : LoKi::Particles::PtFlightWithBestVertex()
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::BremMCorrectedWithBestVertex::~BremMCorrectedWithBestVertex(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::BremMCorrectedWithBestVertex*
LoKi::Particles::BremMCorrectedWithBestVertex::clone() const
{ return new LoKi::Particles::BremMCorrectedWithBestVertex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::BremMCorrectedWithBestVertex::result_type
LoKi::Particles::BremMCorrectedWithBestVertex::operator()
  ( LoKi::Particles::BremMCorrectedWithBestVertex::argument p ) const
{
  if ( 0 == p )
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx )
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv )
  {
    Error("BestVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  setVertex ( pv ) ;
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() ,
           "Vertex-Information is not valid"     ) ;
  //

  //fill m_electrons and m_others respectively with electrons and all other particles
  e_finder(p->clone());
  e_finder_other(p->clone());

  std::cout << m_electron_mothers->size() << "  " << m_rest_electrons->size() << "  " << m_all_electrons->size() << std::endl;

  LorentzVector P_h_tot, P_e_tot;
  LorentzVector P_e_corr_tot, P_e_corr_temp;

  for (const auto child : (*m_others) ) {
    P_h_tot += (child->momentum());
  }

  for (const auto child : (*m_electron_mothers) ) {
    P_e_tot += (child->momentum());
  }

  for (const auto child : (*m_rest_electrons) ) {
    P_e_tot += (child->momentum());
  }

  double pt_h = ptFlight(P_h_tot, p->endVertex()->position(), position());
  double pt_e = ptFlight(P_e_tot, p->endVertex()->position(), position());

  double alpha = pt_h/pt_e;

  for (const auto child : (*m_all_electrons) ) {
    double E_e = sqrt(pow((alpha * child->momentum().X()), 2) + pow((alpha * child->momentum().Y()), 2) + pow((alpha * child->momentum().Z()), 2) + (m_e_PDG*m_e_PDG));
    P_e_corr_temp.SetXYZT (alpha * child->momentum().X(), alpha * child->momentum().Y(), alpha * child->momentum().Z(), E_e);
    P_e_corr_tot += P_e_corr_temp;
  }

  LorentzVector P = P_h_tot + P_e_corr_tot;

  double corr_mass = P.M();

  // Cleanup
  m_rest_electrons->clear() ;
  m_all_electrons->clear() ;
  m_electron_mothers->clear() ;
  m_others->clear() ;

  //return alpha ;
  return corr_mass ;
}

bool LoKi::Particles::BremMCorrectedWithBestVertex::has_only_electrons (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (child->particleID().abspid() != 11) return false ;
  }
  return true ;
}
bool LoKi::Particles::BremMCorrectedWithBestVertex::has_electron (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (!child->isBasicParticle()) {
      if (has_electron(child)) return true ;
    }
    else if (child->particleID().abspid() == 11) return true ;
  }
  return false ;
}
void LoKi::Particles::BremMCorrectedWithBestVertex::e_finder (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (child->isBasicParticle()) {
      if (child->particleID().abspid() == 11) m_all_electrons->push_back(child->clone());
      else m_others->push_back(child->clone());
    }
    else {
      if (has_electron(child)) e_finder(child);
      else m_others->push_back(child->clone());
    }
    if (!child->isBasicParticle() && has_only_electrons(child)) m_electron_mothers->push_back(child->clone());
  }
}
void LoKi::Particles::BremMCorrectedWithBestVertex::e_finder_other (const LHCb::Particle *parent) const {
  for (const auto &child : parent->daughtersVector()) {
    if (!child->isBasicParticle() && has_only_electrons(child)) continue;
    if (child->isBasicParticle()) {
      if (child->particleID().abspid() == 11) m_rest_electrons->push_back(child->clone());
    }
    else if (has_electron(child)) e_finder_other(child);
  }
}

// ============================================================================
//  OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Particles::BremMCorrectedWithBestVertex::fillStream ( std::ostream& s ) const
{ return s << "BPVHOPM" ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================
