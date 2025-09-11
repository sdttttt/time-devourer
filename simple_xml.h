//
// Created by kms on 2025/9/11.
//

using std::wstring;

class CSimpleXML
{
public:
    CSimpleXML(const wstring& xml_path);
    CSimpleXML() = default;
    ~CSimpleXML() = default;

    void LoadXMLContentDirect(const wstring& xml_content) { m_xml_content = xml_content; }

    wstring GetNode(const wchar_t* node, const wchar_t* parent) const;
    wstring GetNode(const wchar_t* node) const;

    static wstring _GetNode(const wchar_t* node, const wstring& content);

protected:
    wstring m_xml_content;

};
