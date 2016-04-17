#include <ciso646>
#include <iostream>
#include "kdtree.hpp"

void nearestNeighbour(kdtree * _cur, vec3 _p, kdtree * _best)
{  
    //DOWN
    //Check the direction we are splitting.
    bool dir = false;
    float curdist = 0.0f;
    if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_x - _cur->m_node->m_pos.m_x);
        if(_p.m_x > _cur->m_node->m_pos.m_x)
        {
            dir = true;
        }
    }
    else if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_y - _cur->m_node->m_pos.m_y);
        if(_p.m_y > _cur->m_node->m_pos.m_y)
        {
            dir = true;
        }
    }
    else if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_z - _cur->m_node->m_pos.m_z);
        if(_p.m_z > _cur->m_node->m_pos.m_z)
        {
            dir = true;
        }
    }
    //If we are not at the end of the tree, keep going.
    if(!dir)
    {
        if(_cur->m_children.first == nullptr)
        {
            *_best = *_cur;
            return;
        }
        nearestNeighbour(_cur->m_children.first, _p, _best);
    }
    else if(dir)
    {
        if(_cur->m_children.second == nullptr)
        {
            *_best = *_cur;
            return;
        }
        nearestNeighbour(_cur->m_children.second, _p, _best);
    }
    //CHECK CURRENT NODE.
    if(magns(_cur->m_node->m_pos - _p) < magns(_best->m_node->m_pos - _p))
    {
        _best = _cur;
    }
    //UP

    //If we went down the first branch...
    if(!dir)
    {
        float childdist = 0.0f;
        if(_cur->m_axis == 0) childdist = fabs(_p.m_x - _cur->m_children.second->m_node->m_pos.m_x);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_y - _cur->m_children.second->m_node->m_pos.m_y);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_z - _cur->m_children.second->m_node->m_pos.m_z);
        //If the OTHER node (one we didn't explore) is closer on the splitting axis...
        if(curdist > childdist) nearestNeighbour(_cur->m_children.second, _p, _best);
    }
    else if(dir)
    {
        float childdist = 0.0f;
        if(_cur->m_axis == 0) childdist = fabs(_p.m_x - _cur->m_children.first->m_node->m_pos.m_x);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_y - _cur->m_children.first->m_node->m_pos.m_y);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_z - _cur->m_children.first->m_node->m_pos.m_z);
        //If the OTHER node (one we didn't explore) is closer on the splitting axis...
        if(curdist > childdist) nearestNeighbour(_cur->m_children.first, _p, _best);
    }
}

void kNearestNeighbours(kdtree * _cur, vec3 _p, std::vector<kdtree *>* _kbest, int _len)
{
    //DOWN
    //Check the direction we are splitting.
    std::cout << "p1" << std::endl;
    bool dir = false;
    float curdist = 0.0f;
    if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_x - _cur->m_node->m_pos.m_x);
        if(_p.m_x > _cur->m_node->m_pos.m_x)
        {
            dir = true;
        }
    }
    else if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_y - _cur->m_node->m_pos.m_y);
        if(_p.m_y > _cur->m_node->m_pos.m_y)
        {
            dir = true;
        }
    }
    else if(_cur->m_axis == 0)
    {
        curdist = fabs(_p.m_z - _cur->m_node->m_pos.m_z);
        if(_p.m_z > _cur->m_node->m_pos.m_z)
        {
            dir = true;
        }
    }
    std::cout << "p2" << std::endl;
    //If we are not at the end of the tree, keep going.
    if(!dir)
    {
        std::cout << "p3" << std::endl;
        if(_cur->m_children.first == nullptr)
        {
            if(_kbest->size() < _len)
            {
                _kbest->push_back(_cur);
            }
            else
            {
                for(auto i = _kbest->begin(); i != _kbest->end(); ++i)
                {
                    kdtree * n = *i;
                    if(magns(_p - _cur->m_node->m_pos) < magns(_p - n->m_node->m_pos))
                    {
                        _kbest->insert(i, _cur);
                        break;
                    }
                }
                _kbest->erase(_kbest->begin() + _len, _kbest->end());
            }
            return;
        }
        kNearestNeighbours(_cur->m_children.first, _p, _kbest, _len);
    }
    else if(dir)
    {
        std::cout << "p4" << std::endl;
        if(_cur->m_children.second == nullptr)
        {
            if(_kbest->size() < _len)
            {
                _kbest->push_back(_cur);
            }
            else
            {
                for(auto i = _kbest->begin(); i != _kbest->end(); ++i)
                {
                    kdtree * n = *i;
                    if(magns(_p - _cur->m_node->m_pos) < magns(_p - n->m_node->m_pos))
                    {
                        _kbest->insert(i, _cur);
                        break;
                    }
                }
                _kbest->erase(_kbest->begin() + _len, _kbest->end());
            }
        }
        kNearestNeighbours(_cur->m_children.second, _p, _kbest, _len);
    }

    std::cout << "p5" << std::endl;
    //CHECK CURRENT NODE.
    if(_kbest->size() < _len)
    {
        std::cout << "p6" << std::endl;
        _kbest->push_back(_cur);
    }
    else
    {
        std::cout << "p7" << std::endl;
        for(auto i = _kbest->begin(); i != _kbest->end(); ++i)
        {
            kdtree * n = *i;
            if(magns(_p - _cur->m_node->m_pos) < magns(_p - n->m_node->m_pos))
            {
                //std::cout << "p7.01" << std::endl;
                _kbest->insert(i, _cur);
                break;
                //std::cout << "p7.02" << std::endl;
            }
        }
        //std::cout << "p7.1" << std::endl;
        _kbest->erase(_kbest->begin() + _len, _kbest->end());
        //std::cout << "p7.2" << std::endl;
    }
    std::cout << "p8" << std::endl;
    //UP

    //If we went down the first branch...
    if(!dir)
    {
        std::cout << "p9" << std::endl;
        float childdist = 0.0f;
        if(_cur->m_axis == 0) childdist = fabs(_p.m_x - _cur->m_children.second->m_node->m_pos.m_x);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_y - _cur->m_children.second->m_node->m_pos.m_y);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_z - _cur->m_children.second->m_node->m_pos.m_z);
        //If the OTHER node (one we didn't explore) is closer on the splitting axis...
        if(curdist > childdist) kNearestNeighbours(_cur->m_children.second, _p, _kbest, _len);
    }
    else if(dir)
    {
        std::cout << "p10" << std::endl;
        float childdist = 0.0f;
        if(_cur->m_axis == 0) childdist = fabs(_p.m_x - _cur->m_children.first->m_node->m_pos.m_x);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_y - _cur->m_children.first->m_node->m_pos.m_y);
        else if(_cur->m_axis == 0) childdist = fabs(_p.m_z - _cur->m_children.first->m_node->m_pos.m_z);
        //If the OTHER node (one we didn't explore) is closer on the splitting axis...
        if(curdist > childdist) kNearestNeighbours(_cur->m_children.first, _p, _kbest, _len);
    }
}
