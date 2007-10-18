/* -*-c++-*- VirtualPlanetBuilder - Copyright (C) 1998-2007 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#include <vpb/BuildOptions>
#include <vpb/Serializer>

#include <iostream>
#include <string>
#include <map>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/Registry>

using namespace vpb;

template<typename C>
class GeospatialExtentsSerializer : public osgDB::Serializer
{
public:

     typedef GeospatialExtents V;
     typedef const V& P;
     typedef P (C::*GetterFunctionType)() const;
     typedef void (C::*SetterFunctionType)(P);

     GeospatialExtentsSerializer(const char* fieldName, P defaultValue, GetterFunctionType getter, SetterFunctionType setter):
        _fieldName(fieldName),
        _default(defaultValue),
        _getter(getter),
        _setter(setter) {}
     
     bool write(osgDB::Output& fw, const osg::Object& obj)
     {
        const C& object = static_cast<const C&>(obj);
        if (fw.getWriteOutDefaultValues() ||
            _default != (object.*_getter)())
        {
            P value = (object.*_getter)();
            fw.indent()<<_fieldName<<" "<<value._min[0]<<" "<<value._min[1]<<" "<<value._max[0]<<" "<<value._max[1]<<std::endl;
        }
     }

    bool read(osgDB::Input& fr, osg::Object& obj, bool& itrAdvanced)
    {
        C& object = static_cast<C&>(obj);
        V value;
        if (fr.read(_fieldName.c_str(), value._min[0], value._min[1], value._max[0], value._max[1]))
        {
            (object.*_setter)(value);
            itrAdvanced = true;
        }
     }
     
     std::string        _fieldName;
     V                  _default;
     GetterFunctionType _getter;
     SetterFunctionType _setter;
};


#define CREATE_ENUM_SERIALIZER(CLASS,PROPERTY,PROTOTYPE) \
    typedef osgDB::EnumSerializer<CLASS, CLASS::PROPERTY> MySerializer;\
    osg::ref_ptr<MySerializer> serializer = new MySerializer(\
        #PROPERTY,\
        PROTOTYPE.get##PROPERTY(),\
        &CLASS::get##PROPERTY,\
        &CLASS::set##PROPERTY\
    )

    
#define ADD_ENUM_PROPERTY(PROPERTY) \
    CREATE_ENUM_SERIALIZER(BuildOptions, PROPERTY, prototype); \
    _serializerList.push_back(serializer.get())

#define ADD_STRING_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_STRING_SERIALIZER(BuildOptions,PROPERTY,prototype))


#define ADD_UINT_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_UINT_SERIALIZER(BuildOptions,PROPERTY,prototype))


#define ADD_INT_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_INT_SERIALIZER(BuildOptions,PROPERTY,prototype))


#define ADD_FLOAT_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_FLOAT_SERIALIZER(BuildOptions,PROPERTY,prototype))


#define ADD_DOUBLE_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_DOUBLE_SERIALIZER(BuildOptions,PROPERTY,prototype))



#define ADD_VEC4_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_VEC4_SERIALIZER(BuildOptions,PROPERTY,prototype))


#define ADD_BOOL_PROPERTY(PROPERTY) _serializerList.push_back(CREATE_BOOL_SERIALIZER(BuildOptions,PROPERTY,prototype))

#define ADD_ENUM_VALUE(VALUE) serializer->add(BuildOptions::VALUE, #VALUE)

#define ADD_ENUM_PROPERTY_TWO_VALUES(PROPERTY,VALUE1,VALUE2) \
    { \
        ADD_ENUM_PROPERTY(PROPERTY);\
        ADD_ENUM_VALUE(VALUE1);\
        ADD_ENUM_VALUE(VALUE2);\
    }

#define ADD_ENUM_PROPERTY_TWO_VALUES(PROPERTY,VALUE1,VALUE2) \
    { \
        ADD_ENUM_PROPERTY(PROPERTY);\
        ADD_ENUM_VALUE(VALUE1);\
        ADD_ENUM_VALUE(VALUE2);\
    }

#define ADD_ENUM_PROPERTY_THREE_VALUES(PROPERTY,VALUE1,VALUE2,VALUE3) \
    { \
        ADD_ENUM_PROPERTY(PROPERTY);\
        ADD_ENUM_VALUE(VALUE1);\
        ADD_ENUM_VALUE(VALUE2);\
        ADD_ENUM_VALUE(VALUE3);\
    }

#define AEV ADD_ENUM_VALUE
#define AEP ADD_ENUM_PROPERTY

class BuildOptionsLookUps
{
public:

    typedef std::list< osg::ref_ptr<osgDB::Serializer> > SerializerList;
    SerializerList _serializerList;

    BuildOptionsLookUps()
    {
        BuildOptions prototype;

        ADD_STRING_PROPERTY(Directory);
        ADD_STRING_PROPERTY(DestinationTileBaseName);
        ADD_STRING_PROPERTY(DestinationTileExtension);
        ADD_STRING_PROPERTY(DestinationImageExtension);
        ADD_STRING_PROPERTY(ArchiveName);
        ADD_STRING_PROPERTY(IntermediateBuildName);
        ADD_STRING_PROPERTY(LogFileName);
        ADD_STRING_PROPERTY(TaskFileName);
        ADD_STRING_PROPERTY(CommentString);
        
        ADD_ENUM_PROPERTY_TWO_VALUES(DatabaseType, LOD_DATABASE, PagedLOD_DATABASE)
        ADD_ENUM_PROPERTY_TWO_VALUES(GeometryType, HEIGHT_FIELD, POLYGONAL)
        ADD_ENUM_PROPERTY_THREE_VALUES(MipMappingMode, NO_MIP_MAPPING, MIP_MAPPING_HARDWARE,MIP_MAPPING_IMAGERY)

        { AEP(TextureType); AEV(RGB_24); AEV(RGBA);AEV(RGB_16);AEV(RGBA_16);AEV(COMPRESSED_TEXTURE);AEV(COMPRESSED_RGBA_TEXTURE); }

        ADD_UINT_PROPERTY(MaximumTileImageSize);
        ADD_UINT_PROPERTY(MaximumTileTerrainSize);

        ADD_FLOAT_PROPERTY(MaximumVisibleDistanceOfTopLevel);
        ADD_FLOAT_PROPERTY(RadiusToMaxVisibleDistanceRatio);
        ADD_FLOAT_PROPERTY(VerticalScale);
        ADD_FLOAT_PROPERTY(SkirtRatio);
        ADD_FLOAT_PROPERTY(MaxAnisotropy);
        
        ADD_BOOL_PROPERTY(BuildOverlays);
        ADD_BOOL_PROPERTY(ReprojectSources);
        ADD_BOOL_PROPERTY(GenerateTiles);
        ADD_BOOL_PROPERTY(ConvertFromGeographicToGeocentric);
        ADD_BOOL_PROPERTY(UseLocalTileTransform);
        ADD_BOOL_PROPERTY(SimplifyTerrain);
        ADD_BOOL_PROPERTY(DecorateGeneratedSceneGraphWithCoordinateSystemNode);
        ADD_BOOL_PROPERTY(DecorateGeneratedSceneGraphWithMultiTextureControl);
        ADD_BOOL_PROPERTY(WriteNodeBeforeSimplification);

        ADD_VEC4_PROPERTY(DefaultColor);
        
        ADD_STRING_PROPERTY(DestinationCoordinateSystem);


        _serializerList.push_back(new GeospatialExtentsSerializer<BuildOptions>(
                "DestinationExtents", 
                prototype.getDestinationExtents(), 
                &BuildOptions::getDestinationExtents,
                &BuildOptions::setDestinationExtents));

        ADD_UINT_PROPERTY(MaximumNumOfLevels);
        
        ADD_BOOL_PROPERTY(RecordSubtileFileNamesOnLeafTile);
        ADD_BOOL_PROPERTY(GenerateSubtile);
        ADD_UINT_PROPERTY(SubtileLevel);
        ADD_UINT_PROPERTY(SubtileX);
        ADD_UINT_PROPERTY(SubtileY);
    }
    
    bool read(osgDB::Input& fr, BuildOptions& db, bool& itrAdvanced)
    {
        for(SerializerList::iterator itr = _serializerList.begin();
            itr != _serializerList.end();
            ++itr)
        {
            (*itr)->read(fr,db, itrAdvanced);
        }
        return true;
    }

    bool write(osgDB::Output& fw, const BuildOptions& db)
    {
        bool result = false;
        for(SerializerList::iterator itr = _serializerList.begin();
            itr != _serializerList.end();
            ++itr)
        {
            if ((*itr)->write(fw,db)) result = true;
        }
        return result;
    }

};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  BuildOptions IO support
//

bool BuildOptions_readLocalData(osg::Object &obj, osgDB::Input &fr);
bool BuildOptions_writeLocalData(const osg::Object &obj, osgDB::Output &fw);

osgDB::TemplateRegisterDotOsgWrapperProxy<BuildOptionsLookUps> BuildOptions_Proxy
(
    new vpb::BuildOptions,
    "BuildOptions",
    "BuildOptions Object",
    BuildOptions_readLocalData,
    BuildOptions_writeLocalData
);


bool BuildOptions_readLocalData(osg::Object& obj, osgDB::Input &fr)
{
    vpb::BuildOptions& gt = static_cast<vpb::BuildOptions&>(obj);
    bool itrAdvanced = false;
    
    BuildOptions_Proxy.read(fr, gt, itrAdvanced);
    
    return itrAdvanced;
}

bool BuildOptions_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
    const vpb::BuildOptions& db = static_cast<const vpb::BuildOptions&>(obj);

    BuildOptions_Proxy.write(fw, db);

    return true;
}
