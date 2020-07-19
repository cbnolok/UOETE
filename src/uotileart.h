#ifndef TILEARTENTRY_H
#define TILEARTENTRY_H

#include <memory>
#include <vector>

namespace uopp {
    class UOPFile;
    class UOPError;
}
class UOStringDictionary;


// - Begin of TileArt sub-fields

struct TAEImgOffset
{
    int xStart, yStart, xEnd, yEnd, xOff, yOff;
};

enum class TAEFlag : unsigned long long
{
    None        = 0x0,  //0uL,
    Background  = 0x1,  //1uL,
    Weapon      = 0x2,  //2uL,
    Transparent = 0x4,  //4uL,
    Translucent = 0x8,  //8uL,
    Wall        = 0x10, //16uL,
    Damaging    = 0x20, //32uL,
    Impassable  = 0x40, //64uL,
    Wet         = 0x80, //128uL,
    Ignored     = 0x100,    //256uL,
    Surface     = 0x200,    //512uL,
    Bridge      = 0x400,    //1024uL,
    Generic     = 0x800,    //2048uL,
    Window      = 0x1000,   //4096uL,
    NoShoot     = 0x2000,   //8192uL,
    ArticleA    = 0x4000,   //16384uL,
    ArticleAn   = 0x8000,   //32768uL,
    ArticleThe  = ArticleA|ArticleAn,  //49152uL,
    Mongen      = 0x10000,  //65536uL,
    Foliage     = 0x20000,  //131072uL,
    PartialHue  = 0x40000,  //262144uL,
    UseNewArt   = 0x80000,      //524288uL,
    Map         = 0x100000,     //1048576uL,
    Container   = 0x200000,     //2097152uL,
    Wearable    = 0x400000,     //4194304uL,
    LightSource = 0x800000,     //8388608uL,
    Animation   = 0x1000000,    //16777216uL,
    HoverOver   = 0x2000000,    //33554432uL,
    ArtUsed     = 0x4000000,    //67108864uL,
    Armor       = 0x8000000,    //134217728uL,
    Roof        = 0x10000000,   //268435456uL,
    Door        = 0x20000000,   //536870912uL,
    StairBack   = 0x40000000,   //1073741824uL,
    StairRight  = 0x80000000,   //2147483648uL,
    NoHouse     = 0x100000000,  //4294967296uL,
    NoDraw      = 0x200000000,  //8589934592uL,
    Unused1     = 0x400000000,  //17179869184uL,
    AlphaBlend  = 0x800000000,  //34359738368uL,
    NoShadow    = 0x1000000000, //68719476736uL,
    PixelBleed  = 0x2000000000, //137438953472uL,
    Unused2     = 0x4000000000, //274877906944uL,
    PlayAnimOnce= 0x8000000000, //549755813888uL,
    MultiMovable= 0x10000000000 //1099511627776uL
};

enum class TAEPropID
{
    Weight = 0,
    Quality,
    Quantity,
    Height,
    Value,
    AcVc,
    Slot,
    Off_C8,
    Appearance,
    Race,
    Gender,
    Paperdoll
};

struct TAEProp
{
    TAEPropID ID;
    unsigned int val;
};

struct TAEStackAlias
{
    //At each amount corresponds a specific id. (Gold/Silver)
    unsigned int amount;
    unsigned int amountID;
};

struct TAEAnimationAppearanceSub3
{
    unsigned int unk1;
    unsigned int unk2;
};
struct TAEAnimationAppearanceSub2
{
    unsigned int subCount;
    std::vector<std::shared_ptr<TAEAnimationAppearanceSub3>> sub3Vector;
};
struct TAEAnimationAppearanceSub1
{
    unsigned int unk1;  // byte
    unsigned int unk2;  // dword
};
struct TAEAnimationAppearance
{
    unsigned int subType;
    std::shared_ptr<TAEAnimationAppearanceSub1> sub1;
    std::shared_ptr<TAEAnimationAppearanceSub2> sub2;
};

struct TAESittingAnimation
{
    unsigned int unk1;
    unsigned int unk2;
    unsigned int unk3;
    unsigned int unk4;
};

struct TAERadarcol
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct TAETextureImage
{
    unsigned int nameStringOff;
    unsigned int unk4;
    float unk5;
    unsigned int unk6;
    unsigned int unk7;
};

struct TAETexture
{
    unsigned int hasTexture;            // byte
    unsigned int unk1;                  // byte
    unsigned int typeStringOff;         // dword (signed?)
    unsigned int textureItemsCount;     // byte
    std::vector<TAETextureImage> textureItems;
    unsigned int unk8Count;             // dword (unsigned)
    std::vector<int> unk8Vector;
    unsigned int unk9Count;             // dword (unsigned)
    std::vector<float> unk9Vector;
};

// - End of TileArt sub-fields


// - Begin of helper data structures

enum class TextureType
{
    Undefined,
    WorldArt,       // Enhanced Client texture
    TileArtLegacy,
    TileArtEnhanced
};
struct TextureItem
{
    TextureType type;
    unsigned int id;
};

// - End of helper data structures


// - TileArtEntry

struct TileArtEntry
{
    static TileArtEntry load(uopp::UOPFile *uopFile, uopp::UOPError *errorQueue, int iBlock, int iFile);
    static std::vector<std::vector<TextureItem>> getTextureItemVector(const std::vector<TAETexture>& TAETextureVector, const UOStringDictionary* stringDictionary);

    int uopBlock, uopFile;

    unsigned int version;       // word
    unsigned int stringDictOff; // dword
    unsigned int tileID;        // dword
    unsigned int unkBool1;      // byte
    unsigned int unkBool2;      // byte
    float unkFloat3;            // dword (float)
    float unkFloat4;            // dword (float)
    int fixedZero;              // dword (signed?)
    int oldID;                  // dword (signed?)
    float unkFloat5;            // dword (float)
    int type;                   // dword (signed?)
    unsigned int unkByte6;      // byte
    int unkDW7;                 // dword (signed/float?), NOT float 3F800000
    int unkDW8;                 // dword (signed?)
    float light1;               // dword (float?)
    float light2;               // dword (float?)
    unsigned int unkDW9;        // dword (signed?)
    unsigned long long flags1;  // qword (unsigned)
    unsigned long long flags2;  // qword (unsigned)
    int facing;                 // dword (signed?)
    TAEImgOffset ECImgOffset;
    TAEImgOffset CCImgOffset;
    unsigned char propCount1;       // byte
    std::vector<TAEProp> propVector1;
    unsigned char propCount2;       // byte
    std::vector<TAEProp> propVector2;

    // This isn't in the tileart file format, but we need this when we'll rewrite the file.
    // Before this offset, we'll write manually the entries.
    // After this offset, we'll copy the older file content to the new file.
    // Doing this way, we'll copy also the data we don't know how to parse, which fortunately is after this offset.
    unsigned int stackAliasCount_offset;

    unsigned int stackAliasCount;   // dword ?
    std::vector<TAEStackAlias> stackAliasVector;
    unsigned int appearanceCount;   // dword ?
    std::vector<TAEAnimationAppearance> appearanceVector;
    unsigned char hasSitting;     // byte
    std::shared_ptr<TAESittingAnimation> sitting;

    unsigned int radarcol_offset;   // save this, we may want to change the radarcol
    TAERadarcol radarcol;

    std::vector<TAETexture> textureVector;
};

/*
struct UOTileArt
{
    UOTileArt(const std::string& clientPath);
    bool load();

    std::string m_clientPath;
    std::map<int, TileArtEntry> m_entries;
    uopp::UOPError m_UOPErrorQueue;
private:
    std::unique_ptr<uopp::UOPPackage> m_UOPPackage;

};
*/

#endif // UOTILEART_H
