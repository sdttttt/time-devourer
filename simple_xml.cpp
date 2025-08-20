//
// Created by kms on 2025/9/11.
//

#include "simple_xml.h"

#include <fstream>
#include <sstream>


BOOL HasUtf8Bom(const std::string_view str)
{
    return str.size() >= 3 &&
        static_cast<unsigned char>(str[0]) == 0xEF &&
        static_cast<unsigned char>(str[1]) == 0xBB &&
        static_cast<unsigned char>(str[2]) == 0xBF;
}

std::wstring UTF82UTF16(const std::string& str)
{
    if (str.empty())
    {
        return L"";
    }

    auto size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);

    if (size_needed <= 0)
    {
        return L"";
    }

    std::wstring ret(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), ret.data(), size_needed);
    return ret;
}


CSimpleXML::CSimpleXML(const wstring& xml_path)
{
    std::ifstream file_stream(xml_path, std::ios::binary);

    if (!file_stream.is_open())
    {
        return;
    }

    //读取文件内容

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string xml_str = buffer.str();

    // UTF-8 BOM（EF BB BF → -17, -69, -65）
    if (HasUtf8Bom(xml_str))
    {
        xml_str.erase(0, 3);
    }

    // 默认全按照UTF8处理
    this->m_xml_content = UTF82UTF16(xml_str);
}

std::wstring CSimpleXML::GetNode(LPCWSTR node, LPCWSTR parent) const
{
    wstring node_content = _GetNode(parent, this->m_xml_content);
    return _GetNode(node, node_content);
}

std::wstring CSimpleXML::GetNode(LPCWSTR node) const
{
    return _GetNode(node, this->m_xml_content);
}

std::wstring CSimpleXML::_GetNode(LPCWSTR node, const wstring& content)
{
    std::wstring node_start{L'<'};
    std::wstring node_end{L'<'};
    node_start += node;
    node_start += L'>';
    node_end += L'/';
    node_end += node;
    node_end += L'>';

    size_t index_start, index_end;

    index_start = content.find(node_start);
    index_end = content.find(node_end);
    if (index_start == wstring::npos || index_end == wstring::npos)
        return wstring();

    auto result = content.substr(index_start + node_start.size(), index_end - index_start - node_start.size());
    return result;
}
