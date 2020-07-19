#include "uotileart.h"

#include <fstream>

#include "uoppackage/uoppackage.h"
#include "uoppackage/uopfile.h"
#include "uostringdictionary.h"

#define DEBUG_MSGS  true


#ifndef DEBUG_MSGS
    #define REPORT(msg)  void(0)
#else
    #include <sstream>
    #include <iostream>
    void __report_func(const std::stringstream& _ss)
    {
        // Useful to have this auxiliary function to place a debugger breakpoint here.
        std::cout << _ss.str() << std::endl;
    }
    #define REPORT(msg)  { std::stringstream _ss; _ss << "[block " << iBlock << ", file " << iFile << ", ID " << entry.tileID << "] " << msg; __report_func(_ss); }
#endif

TileArtEntry TileArtEntry::load(uopp::UOPFile *uopFile, uopp::UOPError* errorQueue, int iBlock, int iFile) // static
{
    // Get the decompressed data from the file packed into tileart.uop
    std::vector<char> decompressedDataVec;
    uopFile->unpack(&decompressedDataVec, errorQueue);

    // Populate a local entry by getting the info from the raw file data
    TileArtEntry entry = {};

    entry.uopBlock = iBlock;
    entry.uopFile = iFile;


    const char* decompressedDataPtr = decompressedDataVec.data();
    size_t dataOffset = 0;

#define READMEMFULL(dest, read_size) \
    if ((dataOffset + read_size) >= decompressedDataVec.size()) { \
        REPORT("Malformed entry at: " #dest); return entry; \
    } \
    memcpy(static_cast<void*>(&(dest)), static_cast<const void*>(decompressedDataPtr+dataOffset), (read_size)); \
    dataOffset += (read_size);

#define READMEM(dest, read_size) \
    READMEMFULL(entry.dest, read_size);


    READMEM(version, 2);
    READMEM(stringDictOff, 4);
    READMEM(tileID, 4);
    READMEM(unkBool1, 1);
    READMEM(unkBool2, 1);
    READMEM(unkFloat3, 4);
    READMEM(unkFloat4, 4);
    READMEM(fixedZero, 4);
    READMEM(oldID, 4);
    READMEM(unkFloat5, 4);
    READMEM(type, 4);
    READMEM(unkByte6, 1);
    READMEM(unkDW7, 4);
    READMEM(unkDW8, 4);
    READMEM(light1, 4);
    READMEM(light2, 4);
    READMEM(unkDW9, 4);

    READMEM(flags1, 8);
    READMEM(flags2, 8);

    READMEM(facing, 4);

    READMEM(ECImgOffset.xStart, 4);
    READMEM(ECImgOffset.yStart, 4);
    READMEM(ECImgOffset.xEnd, 4);
    READMEM(ECImgOffset.yEnd, 4);
    READMEM(ECImgOffset.xOff, 4);
    READMEM(ECImgOffset.yOff, 4);

    READMEM(CCImgOffset.xStart, 4);
    READMEM(CCImgOffset.yStart, 4);
    READMEM(CCImgOffset.xEnd, 4);
    READMEM(CCImgOffset.yEnd, 4);
    READMEM(CCImgOffset.xOff, 4);
    READMEM(CCImgOffset.yOff, 4);

    READMEM(propCount1, 1);
    entry.propVector1.resize(entry.propCount1);
    for (unsigned i = 0; i < entry.propCount1; ++i)
    {
        READMEM(propVector1[i].ID, 1);
        READMEM(propVector1[i].val, 4);
    }
    READMEM(propCount2, 1);
    entry.propVector2.resize(entry.propCount2);
    for (unsigned i = 0; i < entry.propCount2; ++i)
    {
        READMEM(propVector2[i].ID, 1);
        READMEM(propVector2[i].val, 4);
    }

    entry.stackAliasCount_offset = unsigned(dataOffset);
    READMEM(stackAliasCount, 4);
    if (entry.stackAliasCount)
    {
        entry.stackAliasVector.resize(entry.stackAliasCount);
        for (unsigned i = 0; i < entry.stackAliasCount; ++i)
        {
            READMEM(stackAliasVector[i].amount, 4);
            READMEM(stackAliasVector[i].amountID, 4);
        }
    }

    READMEM(appearanceCount, 4);
    if (entry.appearanceCount)
    {
        entry.appearanceVector.resize(entry.appearanceCount);
        for (unsigned x = 0; x < entry.appearanceCount; ++x)
        {
            TAEAnimationAppearance& vecRoot = entry.appearanceVector[x];
            READMEMFULL(vecRoot.subType, 1);
            if (vecRoot.subType == 1)
            {
                vecRoot.sub1 = std::make_shared<TAEAnimationAppearanceSub1>();
                TAEAnimationAppearanceSub1* vecSub1 = vecRoot.sub1.get();
                READMEMFULL(vecSub1->unk1, 1);
                READMEMFULL(vecSub1->unk2, 4);
            }
            else
            {
                vecRoot.sub2 = std::make_shared<TAEAnimationAppearanceSub2>();
                TAEAnimationAppearanceSub2* vecSub2 = vecRoot.sub2.get();
                READMEMFULL(vecSub2->subCount, 4);
                if (vecSub2->subCount)
                {
                    vecSub2->sub3Vector.resize(vecSub2->subCount);
                    for (unsigned y = 0; y < vecSub2->subCount; ++y)
                    {
                        vecSub2->sub3Vector[y] = std::make_shared<TAEAnimationAppearanceSub3>();
                        TAEAnimationAppearanceSub3* vecSub3 = vecSub2->sub3Vector[y].get();
                        READMEMFULL(vecSub3->unk1, 4);
                        READMEMFULL(vecSub3->unk2, 4);
                    }
                }
            }
        }
    }

    READMEM(hasSitting, 1);
    if (entry.hasSitting)
    {
        entry.sitting = std::make_shared<TAESittingAnimation>();
        READMEM(sitting->unk1, 4);
        READMEM(sitting->unk2, 4);
        READMEM(sitting->unk3, 4);
        READMEM(sitting->unk4, 4);
    }

    entry.radarcol_offset = unsigned(dataOffset);
    READMEM(radarcol.r, 1);
    READMEM(radarcol.g, 1);
    READMEM(radarcol.b, 1);
    READMEM(radarcol.a, 1);

    // These are the textures to draw
    entry.textureVector.resize(4);
    for (unsigned i = 0; i < 4; ++i)
    {
        TAETexture& texture = entry.textureVector[i];
        READMEMFULL(texture.hasTexture, 1);
        if (texture.hasTexture)
        {
            if (texture.hasTexture != 1)
            {
                // I don't know how to process the remaining data.
                //  Not too bad, since if i need to rewrite the file, after the radarcol data i'll just do a plain copy of the rest of the data.
                /*
                    std::string msg("Unexpected hasTexture value: "); msg += std::to_string(texture.hasTexture);
                    REPORT(msg.c_str());
                */
                return entry;
            }

            // C7.1
            READMEMFULL(texture.unk1, 1);
            READMEMFULL(texture.typeStringOff, 4);      // shader
            READMEMFULL(texture.textureItemsCount, 1);
            if (texture.textureItemsCount)
            {
                texture.textureItems.resize(texture.textureItemsCount);
                for (unsigned ii = 0; ii < texture.textureItemsCount; ++ii)
                {
                    TAETextureImage& texImage = texture.textureItems[ii];
                    READMEMFULL(texImage.nameStringOff, 4);
                    READMEMFULL(texImage.unk4, 1);
                    READMEMFULL(texImage.unk5, 4);
                    READMEMFULL(texImage.unk6, 4);
                    READMEMFULL(texImage.unk7, 4);
                }
            }

            // C7.2
            READMEMFULL(texture.unk8Count, 4);
            if (texture.unk8Count)
            {
                texture.unk8Vector.resize(texture.unk8Count);
                for (unsigned x = 0; x < texture.unk8Count; ++x)
                {
                    READMEMFULL(texture.unk8Vector[x], 4);
                }
            }

            // C7.3
            READMEMFULL(texture.unk9Count, 4);
            if (texture.unk9Count)
            {
                texture.unk9Vector.resize(texture.unk9Count);
                for (unsigned x = 0; x < texture.unk9Count; ++x)
                {
                    READMEMFULL(texture.unk9Vector[x], 4);
                }
            }

        }
    }

#undef READMEM

    return entry;
}

std::vector<std::vector<TextureItem>> TileArtEntry::getTextureItemVector(const std::vector<TAETexture>& TAETextureVector,
                                                                         const UOStringDictionary* stringDictionary)
{
    std::vector<std::vector<TextureItem>> textureIDs;
    textureIDs.resize(TAETextureVector.size());

    // Loop through the textures and find the texture ids to show (they are stored inside string_dictionary.uop)
    for (unsigned i = 0; i < TAETextureVector.size(); ++i)
    {
        if (!TAETextureVector[i].hasTexture)
            continue;

        //qDebug() << "shader" << m_stringDictionary.getString(TAETextureVector[i].typeStringOff - 1).c_str();

        textureIDs[i].resize(TAETextureVector[i].textureItemsCount);
        for (unsigned ii = 0; ii < TAETextureVector[i].textureItemsCount; ++ii)
        {
            const TAETextureImage& textureItem = TAETextureVector[i].textureItems[ii];
            const std::string& str = stringDictionary->getString(textureItem.nameStringOff - 1);

            // Then it's something like "Data\WorldArt\00000002_ankh.tga". This is funny for a number of reasons:
            // 1) The actual extension is dds, not tga.
            // 2) The actual path is "xxx/yyy/********.dds". Notice the different slashes, the different folder, the different
            //  file name and the different extension.
            // 3) It seems it's the only reference to the id of the linked texture to show. And it's a string, not a numeric field. sob.

            if (std::string::npos != str.find("Data\\WorldArt\\"))
                textureIDs[i][ii].type = TextureType::WorldArt;
            else if (std::string::npos != str.find("Data\\TileArtLegacy\\"))
                textureIDs[i][ii].type = TextureType::TileArtLegacy;
            else if (std::string::npos != str.find("Data\\TileArtEnhanced\\"))
                textureIDs[i][ii].type = TextureType::TileArtEnhanced;

            size_t iStart = str.find_first_of('0');
            size_t iEnd = str.find_first_of('_', iStart);
            if ((iStart == std::string::npos) || (iEnd == std::string::npos))
            {
                // Data\TileArtLegacy\3.tga
                iStart = str.find_last_of('\\');
                iEnd = str.find_first_of('.', iStart);
                if ((iStart == std::string::npos) || (iEnd == std::string::npos))
                    continue;
                ++iStart;
            }
            std::string strTrimmed = str.substr(iStart, iEnd - iStart);
            try
            {
                textureIDs[i][ii].id = static_cast<unsigned int>(std::stol(strTrimmed));
            }
            catch (std::invalid_argument&) { /*qDebug() << "Invalid shader" << strTrimmed.c_str();*/ }
        }
    }
    return textureIDs;
}


/*
UOTileArt::UOTileArt(const std::string& clientPath),
m_clientPath(clientPath)
{
}
*/
