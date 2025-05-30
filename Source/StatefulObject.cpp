#include "StatefulObject.h"

StatefulObject::StatefulObject(const String& nameScope, const String& namePrefix) : NamedObject(nameScope, namePrefix), deleteStateWhenDestroyed(true)
{
    initTree();
}

StatefulObject::StatefulObject(const String& uniqueName) : NamedObject(uniqueName), deleteStateWhenDestroyed(true)
{
    initTree();
}

void StatefulObject::initTree() {
    children.minimiseStorageOverheads();
    valueTree = ValueTree(createID());
    valueTree.addListener(this);
}

StatefulObject::StatefulObject(StatefulObject& parent, const String& nameScope, const String& namePrefix, bool deleteStateWhenDestroyed) : NamedObject(nameScope, namePrefix), deleteStateWhenDestroyed(deleteStateWhenDestroyed)
{
    initTreeFromParent(parent);
}

StatefulObject::StatefulObject(StatefulObject& parent, const String& uniqueName, bool deleteStateWhenDestroyed) : NamedObject(uniqueName), deleteStateWhenDestroyed(deleteStateWhenDestroyed)
{
    initTreeFromParent(parent);
}

void StatefulObject::initTreeFromParent(StatefulObject &parent) {
    children.minimiseStorageOverheads();
    parent.addChild(this);
    valueTree = parent.valueTree.getOrCreateChildWithName(createID(), nullptr);
    this->parent = &parent;
    valueTree.addListener(this);
}

StatefulObject::StatefulObject(StatefulObject& parent, const ObjectState& state, bool deleteStateWhenDestroyed) : NamedObject(state.getName()), valueTree(state.getTree()), deleteStateWhenDestroyed(deleteStateWhenDestroyed)
{
    children.minimiseStorageOverheads();
    parent.addChild(this);
    valueTree.addListener(this);
}

StatefulObject::~StatefulObject() {
    if(deleteStateWhenDestroyed) {
        valueTree.getParent().removeChild(valueTree, nullptr);
    }
    if(parent != nullptr) {
        parent->deleteChild(this);
    }
}
//---------------------------------------------------------
Array<StatefulObject::ObjectState> StatefulObject::getChildrenStates() const {
    Array<ObjectState> arr;
    
    ValueTree::Iterator iter = valueTree.begin();
    while(iter != valueTree.end()) {
        arr.add(ObjectState(*iter, createName(getName(), (*iter).getType())));
        ++iter;
    }
    
    return arr;
}

StatefulObject::ObjectState StatefulObject::getChildState(const Identifier &identifier) const {
    ValueTree childTree = valueTree.getChildWithName(identifier);
    return ObjectState(childTree, createName(getName(), identifier));
}

void StatefulObject::saveState(const String& filename) const {
    File file(filename); // need premissions
    saveState(file);
}

void StatefulObject::restoreState(const String& filename) {
    File file(filename); // need premissions
    restoreState(file);
}

void StatefulObject::saveState(File& file) const {
    std::unique_ptr<XmlElement> xml = valueTree.createXml();
    xml->writeTo(file);
}

void StatefulObject::restoreState(File& file) {
    if(file.existsAsFile()) {
        std::unique_ptr<XmlElement> xml = XmlDocument::parse(file);
        ValueTree restoreTree = ValueTree::fromXml(*xml.get());
        
        if(restoreTree.getType() == valueTree.getType()) {
            valueTree = restoreTree;
        } else {
            throw StateException("Incorrect state file.");
        }
    } else {
        file.create();
        valueTree = ValueTree(createID());
    }
}

void StatefulObject::rename(const String& newName) {
    String nameToRollback = getName();
    
    if(newName == nameToRollback) {
        return;
    }
    
    if(newName.contains(terminator)) {
        throw StateException("Please, do not use string \"" + terminator + "\" in names. It uses internally.");
    }
    
    NamedObject::rename(newName);
    
    Identifier newIdentifier;
    try {
        newIdentifier = createID();
    } catch (const StateException& e) {
        NamedObject::rename(nameToRollback);
        throw e;
    }
    
    ValueTree parentTree = valueTree.getParent();
    int index = parentTree.indexOf(valueTree);
    ValueTree newValueTree{newIdentifier};
    newValueTree.copyPropertiesAndChildrenFrom(valueTree, nullptr);
    valueTree.removeListener(this);
    parentTree.removeChild(valueTree, nullptr);
    parentTree.addChild(newValueTree, index, nullptr);
    valueTree = newValueTree;
    valueTree.addListener(this);
    
    for(int i = 0; i < children.size(); ++i) {
        children[i]->parentRenamed(valueTree);
    }
}

void StatefulObject::move(int newIdx) {
    ValueTree parent = valueTree.getParent();
    int currentIdx = parent.indexOf(valueTree);
    parent.moveChild(currentIdx, newIdx, nullptr);
}

//---------------------------------------------------------
void StatefulObject::setProperty(const Identifier &name, const var &newValue) {
    valueTree.setProperty(name, newValue, nullptr);
}

const var& StatefulObject::getProperty(const Identifier &name) const {
    return valueTree.getProperty(name);
}

void StatefulObject::removeProperty(const Identifier &name) {
    valueTree.removeProperty(name, nullptr);
}

bool StatefulObject::hasProperty(const Identifier& name) const {
    return valueTree.hasProperty(name);
}

void StatefulObject::setPropertyIfNotExists(const Identifier &name, const var &newValue) {
    if(!valueTree.hasProperty(name)) {
        setProperty(name, newValue);
    }
}

const var& StatefulObject::getProperty(const Identifier &name, var defaultValue) {
    setPropertyIfNotExists(name, defaultValue);
    return getProperty(name);
}

Value StatefulObject::getPropertyAsValue(const Identifier &name) {
    return valueTree.getPropertyAsValue(name, nullptr);
}

bool StatefulObject::hasChildren() const {
    return valueTree.getNumChildren() != 0;
}

bool StatefulObject::hasChild(const Identifier &identifier) const {
    return valueTree.getChildWithName(identifier).isValid();
}
//---------------------------------------------------------
Identifier StatefulObject::createID() {
    String possibleID = getName().replace(" ", terminator);
    if(!XmlElement::isValidXmlName(possibleID)) {
        possibleID = terminator + possibleID;
        if(!XmlElement::isValidXmlName(possibleID)) {
            throw StateException("Name \"" + getName() + "\" is not valid!");
        }
    }
    return Identifier(possibleID);
}

NamedObject::Name StatefulObject::createName(const String& scope, const Identifier& identifier) {
    String possibleNameStr = identifier.toString().replace(terminator, " ").trim();
    Name tmpName(scope, Uuid().toString());
    return validateAndCreateCustomName(tmpName, possibleNameStr);
}
