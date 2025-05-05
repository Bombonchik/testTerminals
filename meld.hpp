#ifndef MELD_HPP
#define MELD_HPP
#include <vector>
#include <optional>
#include <cereal/types/vector.hpp> // Include for vector serialization
#include <cereal/types/optional.hpp> // Include for optional serialization
#include "card.hpp" // Assuming card.hpp defines the Card class

struct MeldRequest {
    std::vector<Card> cards;
    std::optional<Rank> addToRank;
      // - nullopt = “I want to initialize a NEW meld”
      // - Rank    = “I want to add these cards to the existing meld of rank addToRank”

    // Add Cereal serialize method
    template <class Archive>
    void serialize(Archive& ar) {
        // Need to serialize Rank enum as underlying type (e.g., int)
        // Assuming Rank has a Cereal serialization function defined elsewhere
        ar(CEREAL_NVP(cards), CEREAL_NVP(addToRank));
    }
};

#endif // MELD_HPP