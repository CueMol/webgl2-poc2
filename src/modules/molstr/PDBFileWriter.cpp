// -*-Mode: C++;-*-
//
// PDB coordinate reader
//
// $Id: PDBFileWriter.cpp,v 1.8 2011/04/16 07:40:51 rishitani Exp $
//

#include <common.h>

#include "PDBFileWriter.hpp"
#include "PDBFileReader.hpp"

#include <qlib/PrintStream.hpp>
#include <qlib/LChar.hpp>
#include <qlib/ClassRegistry.hpp>
#include <qlib/LClassUtils.hpp>

#include "MolResidue.hpp"
#include "MolCoord.hpp"
#include "MolChain.hpp"
#include "MolAtom.hpp"
#include "ResidIterator.hpp"

#define CCP4_CONV 1
//#define QTL_CONV 1

using namespace molstr;
using qlib::LChar;

// MC_DYNCLASS_IMPL(PDBFileWriter, PDBFileWriter, qlib::LSpecificClass<PDBFileWriter>);

PDBFileWriter::PDBFileWriter()
{
}

PDBFileWriter::~PDBFileWriter()
{
}

void PDBFileWriter::attach(qsys::ObjectPtr pMol)
{
  if (!canHandle(pMol)) {
    MB_THROW(qlib::InvalidCastException, "PDBFileWriter");
    return;
  }
  super_t::attach(pMol);
}

/// get file-type description
const char * PDBFileWriter::getTypeDescr() const
{
  return "PDB coordinates (*.pdb)";
}

/// get file extension
const char * PDBFileWriter::getFileExt() const
{
  return "*.pdb";
}

/// get nickname for scripting
const char *PDBFileWriter::getName() const
{
  return "pdb";
}

bool PDBFileWriter::canHandle(qsys::ObjectPtr pobj) const
{
  return (dynamic_cast<MolCoord *>(pobj.get())!=NULL);
}

/////////

namespace {
  char convChainName(const LString &chnam)
  {
    LString rval = chnam;
    if (chnam.equals("none") || chnam.equals("_"))
      rval = " ";
    // rval = rval.toUpperCase();
    return rval.getAt(0);
  }
}

// write PDB file to stream
bool PDBFileWriter::write(qlib::OutStream &outs)
{
  m_pMol = getTarget<MolCoord>();

  if (m_pMol==NULL) {
    LOG_DPRINTLN("PDBWriter> MolCoord is not attached !!");
    return false;
  }

  // check extension record handlers
  PDBFileReader::HndlrTab &htab = PDBFileReader::m_htab;
  bool bUseHndlr = htab.size()>0;

  MolCoord *pMol = m_pMol;
  qlib::PrintStream prs(outs);

  //
  //  write header
  //
  //LString sbuf = pqsys->getVersion();
  //prs.formatln("REMARK   PDB File Generated by CueMol (ver %s)", sbuf.c_str());
  prs.formatln("REMARK   PDB File Generated by CueMol2");

  //
  //  write SSBOND record
  //
  writeSSBonds(prs);

  //
  //  write LINK record
  //
  writeLinks(prs);

  writeSecstr(prs);

  //
  //  write extension records (CRYST1)
  //
  if (bUseHndlr) {
    LString sbuf;
    PDBFileReader::HndlrTab::const_iterator iter = htab.begin();
    for (; iter!=htab.end(); ++iter) {
      PDBFileReader::RecordHandler *ph = iter->second;
      if (ph!=NULL && ph->write(sbuf, pMol)) {
        prs.println(sbuf);
      }
    }
  }
  
  //
  //  write body (ATOM/ANISOU records)
  //

  int nserial = 1;

  // Sort chain names by ASCII order
  // take care of '_' (empty) chain
  std::list<LString> chnames;
  {
    MolCoord::ChainIter iter = pMol->begin();
    bool bHasEmptyChain = false;
    for (; iter!=pMol->end(); ++iter) {
      MolChainPtr pChn = iter->second;
      LString chnam = (pChn->getName().c_str()); 
      if (chnam.equals("_")) {
        bHasEmptyChain = true;
        continue;
      }
      chnames.push_back(chnam);
    }
    chnames.sort();
    if (bHasEmptyChain)
      chnames.push_back("_");
  }
  
  std::list<LString>::const_iterator cniter = chnames.begin();
  for (; cniter!=chnames.end(); ++cniter) {
    LString chnam = *cniter;
    MolChainPtr pChn = pMol->getChain(chnam);

    // format chain name
    char cch = convChainName(chnam);

    LString resnam;
    MolChain::ResidCursor2 riter = pChn->begin2();
    // int nlastres = 0;
    for (; riter!=pChn->end2(); ++riter) {
      //MolResiduePtr pRes = *riter;
      MolResiduePtr pRes = riter->second;
      if (pRes.isnull()) continue;
      ResidIndex rindex = pRes->getIndex();
      resnam = pRes->getName();
      
      // format residue name
      // resnam = resnam.toUpperCase();
      resnam = resnam.substr(0,3);

      // sort atom by AID
      std::list<int> atmlist;
      {
        MolResidue::AtomCursor aiter = pRes->atomBegin();
        for (; aiter!=pRes->atomEnd(); ++aiter) {
          atmlist.push_back(aiter->second);
        }
        atmlist.sort();
      }

      std::list<int>::const_iterator iter = atmlist.begin();
      for (; iter!=atmlist.end(); ++iter) {
        int aid = *iter;
        MolAtomPtr pAtm = pMol->getAtom(aid);
        if (pAtm.isnull())
          continue;

        if (!m_pSel.isnull()) {
          if (!m_pSel->isSelected(pAtm))
            continue; // skip nonselected atom
        }

        writeAtomLine(nserial, rindex, resnam,
                      cch, pAtm, prs);
        nserial++;
      }
      // nlastres = rindex.first;
    }

    // print TER record
    /*prs.formatln("TER   "
                 "%5d"
                 "      "
                 "%3s "
                 "%1s"
                 "%4d"
                 "                                                      ",
                 nserial, resnam.c_str(), chnam.c_str(), nlastres);*/
    prs.println("TER");
    nserial ++;
  }  

  //prs.println("END                                                                             ");
  prs.println("END");
  return true;
}

LString PDBFileWriter::formatAtomName(MolAtomPtr pAtom)
{
  LString atomnam = pAtom->getName();
  char cConfID = pAtom->getConfID();
  int elem = pAtom->getElement();
  
  if (cConfID=='\0')
    cConfID = ' ';

  // invalid name case
  if (atomnam.length()>=4||
      elem==ElemSym::XX) {
    return LString::format("%4s%c", atomnam.c_str(), cConfID);
  }

  LString elenam = ElemSym::symID2Str(elem);
  elenam = elenam.toUpperCase();
  int elepos = atomnam.indexOf(elenam);
  if (elepos<0) {
    return LString::format("%4s%c", atomnam.c_str(), cConfID);
  }
  
  LString atommod;
  // if (atomnam.equals(elenam)) {
  // // atom name==elem name
  // shead += LString::format(" %2s  ", elenam.c_str());
  // break;
  // }
  
  elepos += elenam.length();
  atommod = atomnam.substr(elepos);
  elenam = atomnam.substr(0, elepos);
  
  if (atommod.length()<=2) {
    return LString::format("%2s%-2s%c",
                           elenam.c_str(), atommod.c_str(), cConfID);
  }
  
  return LString::format("%4s%c", atomnam.c_str(), cConfID);
}

bool PDBFileWriter::writeAtomLine(int nserial, const ResidIndex &rindex,
                                  const char *resnam, char chainch,
                                  MolAtomPtr pa, qlib::PrintStream &prs)
{
  int resind = rindex.first;
  char inscode = rindex.second;
  LString atomnam = pa->getName().c_str();
  // atomnam = atomnam.toUpperCase();

  // conv ILE's CD name
  // (CD is converted to CD1 in PDBFileReader, so this should not occur)
  if (LChar::equals(resnam, "ILE") && atomnam.equals("CD"))
    atomnam = "CD1";

#ifdef QTL_CONV
  // conv nucl's prime to aster
  atomnam.replace('\'', '*');

  // convert THY's C5A to C5M
  if (LChar::equals(resnam, "THY") &&
      atomnam.equals("C5A")) {
    atomnam = "C5M";
  }

  // conv nucl name
  if (LChar::equals(resnam, "ADE")) {
    resnam = "  A";
  }
  else if (LChar::equals(resnam, "THY")) {
    resnam = "  T";
  }
  else if (LChar::equals(resnam, "GUA")) {
    resnam = "  G";
  }
  else if (LChar::equals(resnam, "CYT")) {
    resnam = "  C";
  }
  else if (LChar::equals(resnam, "URI")) {
    resnam = "  U";
  }
#endif

  LString shead;

  shead = LString::format("%5d ", nserial);

  // format atom name
  shead += formatAtomName(pa);

  shead += LString::format("%3s "
                           "%c"
                           "%4d"
                           "%c",
                           resnam,
                           chainch,
                           resind,
                           (inscode=='\0') ? ' ' : inscode);
                           
  //////////
  // output to the stream

  prs.print("ATOM  ");
  prs.print(shead);

  // Get atom position before applying xformMat, if xformMat is set
  Vector4D pos = pa->getRawPos();

  prs.formatln("   "
               "%8.3f"
               "%8.3f"
               "%8.3f"
               "%6.2f"
               "%6.2f"
               "          "
               "    ",
               pos.x(),
               pos.y(),
               pos.z(),
               pa->getOcc(),
               pa->getBfac());

  if (pa->hasAnIsoU()) {
    prs.print("ANISOU");
    prs.print(shead);

    int u11 = int(pa->getU(0,0) * 1.0e4);
    int u22 = int(pa->getU(1,1) * 1.0e4);
    int u33 = int(pa->getU(2,2) * 1.0e4);
    int u12 = int(pa->getU(0,1) * 1.0e4);
    int u13 = int(pa->getU(0,2) * 1.0e4);
    int u23 = int(pa->getU(1,2) * 1.0e4);

    prs.formatln(" "
                 " %6d"
                 " %6d"
                 " %6d"
                 " %6d"
                 " %6d"
                 " %6d"
                 "          ",
                 u11, u22, u33, u12, u13, u23);
  }

  return true;
}

void PDBFileWriter::writeSSBonds(qlib::PrintStream &prs)
{
  MolCoord::BondIter iter = m_pMol->beginBond();
  MolCoord::BondIter iend = m_pMol->endBond();

  int ind = 1;
  for (; iter!=iend; ++iter) {
    MolBond *pBond = iter->second;
    if (!pBond->isPersist())
      continue;

    MolAtomPtr pAtom1 = m_pMol->getAtom(pBond->getAtom1());
    MolAtomPtr pAtom2 = m_pMol->getAtom(pBond->getAtom2());
    MB_ASSERT(!pAtom1.isnull());
    MB_ASSERT(!pAtom2.isnull());

    if (!pAtom1->getResName().equals("CYS") ||
        !pAtom2->getResName().equals("CYS"))
      continue;

    if (!pAtom1->getName().equals("SG") ||
        !pAtom2->getName().equals("SG"))
      continue;

    char chain = convChainName(pAtom1->getChainName());
    ResidIndex resix = pAtom1->getResIndex();
    char ins = resix.second;
    if (ins=='\0') ins = ' ';
    prs.format("SSBOND"
               " %3d"
               " CYS"
               " %c"
               "%5d"
               "%c",
               ind, chain, resix.first, ins);

    chain = convChainName(pAtom2->getChainName());
    resix = pAtom2->getResIndex();
    ins = resix.second;
    if (ins=='\0') ins = ' ';
    prs.format("   CYS"
               " %c"
               "%5d"
               "%c",
               chain, resix.first, ins);
    prs.println("                         1555   1555       ");
    ++ind;
  }
}

void PDBFileWriter::writeLinks(qlib::PrintStream &prs)
{
  MolCoord::BondIter iter = m_pMol->beginBond();
  MolCoord::BondIter iend = m_pMol->endBond();

  int ind = 1;
  for (; iter!=iend; ++iter) {
    MolBond *pBond = iter->second;
    if (!pBond->isPersist())
      continue;

    MolAtomPtr pAtom1 = m_pMol->getAtom(pBond->getAtom1());
    MolAtomPtr pAtom2 = m_pMol->getAtom(pBond->getAtom2());
    MB_ASSERT(!pAtom1.isnull());
    MB_ASSERT(!pAtom2.isnull());

    LString resn1 = pAtom1->getResName();
    LString resn2 = pAtom2->getResName();
    LString aname1 = pAtom1->getName();
    LString aname2 = pAtom2->getName();

    if (resn1.equals("CYS") && resn2.equals("CYS") &&
        aname1.equals("SG") && aname2.equals("SG"))
      continue; // skip ssbond

    char chain = convChainName(pAtom1->getChainName());
    ResidIndex resix = pAtom1->getResIndex();
    char ins = resix.second;
    if (ins=='\0') ins = ' ';
    prs.print("LINK  "
              "      ");
    prs.print(formatAtomName(pAtom1));
    prs.format("%3s"          
               " %c"
               "%4d"
               "%c",
               resn1.c_str(),
               chain,
               resix.first,
               ins);

    prs.print("               ");

    chain = convChainName(pAtom2->getChainName());
    resix = pAtom2->getResIndex();
    ins = resix.second;
    if (ins=='\0') ins = ' ';

    prs.print(formatAtomName(pAtom2));
    prs.format("%3s"          
               " %c"
               "%4d"
               "%c",
               resn2.c_str(),
               chain,
               resix.first,
               ins);

    prs.println("  "
                "  1555 "
                "  1555"
                "      ");
    ++ind;
  }
}

void PDBFileWriter::writeSecstr(qlib::PrintStream &prs)
{
  MolResiduePtr pRes1;

  int nhx = 1;
  MolCoordPtr pMol(m_pMol);
  ResidIterator riter(pMol); //(MolCoordPtr(m_pMol));

  // Write HELIX records
  for (riter.first(); riter.hasMore(); riter.next()) {
    LString sec;
    LString pfx;
    MolResiduePtr pRes = riter.get();
    pRes->getPropStr("secondary2", sec);

    // MB_DPRINTLN("%s%d => %s", pRes->getChainName().c_str(), pRes->getIndex().first, sec.c_str());

    if (sec.length()>=2)
      pfx= sec.substr(1,1);
    
    if (!(sec.startsWith("H")||sec.startsWith("G")||sec.startsWith("I")))
      continue;

    if (pfx.equals("s"))
      pRes1 = pRes;
    else if (pfx.equals("e")) {
      LString resn1 = pRes1->getName().substr(0,3);
      LString chn1 = pRes1->getChainName().substr(0,1);
      ResidIndex resix1 = pRes1->getIndex();
      char ins1 = resix1.second;
        if (ins1=='\0') ins1 = ' ';
      
      LString resn2 = pRes->getName().substr(0,3);
      LString chn2 = pRes->getChainName().substr(0,1);
      ResidIndex resix2 = pRes->getIndex();
      char ins2 = resix2.second;
      if (ins2=='\0') ins2 = ' ';
      
      prs.print("HELIX ");
      // helix seqno
      prs.format(" %3d", nhx);
        // helix ID
      prs.format(" %3d", nhx);
      
      // start resname
      prs.format(" %3s", resn1.c_str());
      prs.format(" %1s", chn1.c_str());
      prs.format(" %4d", resix1.first);
      prs.format("%c", ins1);
      
      // end resname
      prs.format(" %3s", resn2.c_str());
      prs.format(" %1s", chn2.c_str());
      prs.format(" %4d", resix2.first);
      prs.format("%c", ins2);
      
      // typeof helix
      int ntype = 1;
      if (sec.startsWith("G"))
        ntype = 5;
      else if (sec.startsWith("I"))
        ntype = 3;
      prs.format("%2d", ntype);
      
      prs.println("");
      ++nhx;
    }
  }

  // Write SHEET records
  int nsh = 1;
  for (riter.first(); riter.hasMore(); riter.next()) {
    LString sec;
    LString pfx;
    MolResiduePtr pRes = riter.get();
    pRes->getPropStr("secondary2", sec);

    // MB_DPRINTLN("%s%d => %s", pRes->getChainName().c_str(), pRes->getIndex().first, sec.c_str());

    if (sec.length()>=2)
      pfx= sec.substr(1,1);
    
    if (!(sec.startsWith("E")))
      continue;

    if (pfx.equals("s"))
      pRes1 = pRes;
    else if (pfx.equals("e")) {
      
      LString resn1 = pRes1->getName().substr(0,3);
      LString chn1 = pRes1->getChainName().substr(0,1);
      ResidIndex resix1 = pRes1->getIndex();
      char ins1 = resix1.second;
      if (ins1=='\0') ins1 = ' ';
      
      LString resn2 = pRes->getName().substr(0,3);
      LString chn2 = pRes->getChainName().substr(0,1);
      ResidIndex resix2 = pRes->getIndex();
      char ins2 = resix2.second;
      if (ins2=='\0') ins2 = ' ';
      
      prs.print("SHEET ");
      // helix seqno
      prs.format(" %3d", nsh);
      // helix ID
      prs.format(" %3d", nsh);
      // num of strands
      prs.print(" 1");

      // start resname
      prs.format(" %3s", resn1.c_str());
      prs.format(" %1s", chn1.c_str());
      prs.format("%4d", resix1.first);
      prs.format("%c", ins1);
      
      // end resname
      prs.format(" %3s", resn2.c_str());
      prs.format(" %1s", chn2.c_str());
      prs.format("%4d", resix2.first);
      prs.format("%c", ins2);
      
      // direction
      prs.print(" 0");

      prs.println("");
      ++nsh;
    }
  } // for

}

