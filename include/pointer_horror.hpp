#ifndef MOOSS_POINTER_HORROR
#define MOOSS_POINTER_HORROR


template< class inside, std::size_t S>
const inside* fetch_data_ptr(const std::array<inside, S>& container)
{
    return container.data();
}

template< class inside>
const inside* fetch_data_ptr(const glm::tvec3<inside>& container)
{
    return glm::value_ptr(container);
}

template< class inside, std::size_t S>
std::size_t dimension(const std::array<inside, S>& container)
{
    return S;
}

template<class inside>
std::size_t dimension(glm::tvec3<inside> const& container)
{
    return 3;
}

template<class outside>
std::size_t fetch_data_size( const outside& container)
{
    return sizeof(container);
}
template<class inside>
std::size_t fetch_data_size( std::vector<inside> const& container )
{
    return fetch_data_size(container.front()) * container.size();
}



/////////////////////////////////////////////
// containors of containors generalisation //
/////////////////////////////////////////////

//todo: find how to generalize this to n dimensions (finding the leaf type)
template<typename outside>
const typename outside::value_type::value_type* fetch_data_ptr(const outside& container)
{
    return fetch_data_ptr(container.front());
}

template<typename outside>
std::size_t dimension(const outside& container)
{
    return dimension(container.front());
}


#endif//MOOSS_POINTER_HORROR
