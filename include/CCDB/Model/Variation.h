
/*
 * Variation.h
 *
 *  Created on: Sep 15, 2010
 *      Author: romanov
 */

#ifndef VARIATION_H_
#define VARIATION_H_

#include <string>
#include <ctime>


namespace ccdb
{

    class Variation
    {
    public:
        Variation();

        unsigned int GetId() const { return mId; }                      /// get database table uniq id;
        void SetId(unsigned int val){ mId = val; }                      /// set database table uniq id;

        std::string GetName() const { return mName; }                   /// get name
        void SetName(const std::string&val) { mName = val; }                  /// set name

        std::string GetComment() const { return mComment; }             /// get comment
        void SetComment(const std::string&val)  { mComment = val; }           /// set comment

        time_t GetCreatedTime() const { return mCreatedTime; }          /// get mCreatedTime time
        void SetCreatedTime(time_t val) { mCreatedTime = val; }         /// set mCreatedTime time

        time_t GetModifiedTime() const { return mUpdateTime; }          /// get mUpdateTime time
        void SetModifiedTime(time_t val) { mUpdateTime = val; }         /// set mUpdateTime time

        std::string GetDescription() const { return mDescription; }     /// get description
        void SetDescription(const std::string&val) { mDescription = val; }    /// get description

        std::shared_ptr<Variation> GetParent() const { return mParent; }
        void SetParent(std::shared_ptr<Variation> val) { mParent = val; }

        unsigned int GetParentDbId() const { return mParentDbId; }
        void SetParentDbId(unsigned int val) { mParentDbId = val; }

    protected:

    private:
        unsigned int mId;            //! database table uniq id;
        unsigned int mParentDbId;      /// Database id of parent variation

        std::shared_ptr<Variation> mParent;      /// Get parent variation

        string mName;        //! name
        string mComment;        //! comment
        time_t mCreatedTime;    //! Creation Time
        time_t mUpdateTime;    //! Update Time
        string mDescription;    //! description
    };

}

#endif /* VARIATION_H_ */
