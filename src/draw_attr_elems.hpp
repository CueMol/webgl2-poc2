#pragma once

#include <vector>

namespace gfx {

constexpr int FLOAT32 = 0;

struct AttrInfo
{
    std::string attrName;
    int nAttrElems;
    int iAttrType;
    int nStartPos;
};

// template <class _IndType, class _ElemType>
template <class _ElemType>
class DrawAttrArray
{
private:
    std::vector<AttrInfo> m_attrs;
    void* m_pdata;
    size_t m_nSize;

public:
    DrawAttrArray() : m_pdata(nullptr), m_nSize(0) {}
    virtual ~DrawAttrArray() {}

    using elem_t = _ElemType;

    inline void setAttrSize(size_t nsz) {
        m_attrs.resize(nsz);
    }
    inline size_t getAttrSize() const {
        return m_attrs.size();
    }

    inline void setAttrInfo(int ind, const std::string &name, int ae, int at, int pos) {
        m_attrs[ind].attrName = name;
        m_attrs[ind].nAttrElems = ae;
        m_attrs[ind].iAttrType = at;
        m_attrs[ind].nStartPos = pos;
    }

    inline std::string getAttrName(size_t ind) const {
        return m_attrs[ind].attrName;
    }
    inline int getAttrElemSize(size_t ind) const {
        return m_attrs[ind].nAttrElems;
    }
    inline int getAttrTypeID(size_t ind) const {
        return m_attrs[ind].iAttrType;
    }
    inline int getAttrPos(size_t ind) const {
        return m_attrs[ind].nStartPos;
    }

    //////////

    size_t getSize() const { return m_nSize; }
    void setSize(size_t n) { m_nSize = n; }

    /// returns attribute buffer ptr
    const void *getData() const {
      return m_pdata;
    }

    void *getData() {
      return m_pdata;
    }

    const elem_t &at(int i) const {
        const elem_t *p = static_cast<const elem_t *>(getData());
        return p[i];
    }

    elem_t &at(int i) {
        elem_t *p = static_cast<elem_t *>(getData());
        return p[i];
    }

    size_t getElemSize() const {
        return sizeof(_ElemType);
    }

    /// returns attribute buffer size (in byte unit)
    inline size_t getDataSize() const {
        return getElemSize() * getSize();
    }

};

}
