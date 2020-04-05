#include "./../header/cfg.h"

configuration_t init(const int& argc, const char* argv[])
{
    if (argc > 2)
        throw std::runtime_error("Argument error");
    std::string cfg_filepath;
    cfg_filepath = argc == 2 ? argv[1] : "../config.dat";
    std::ifstream config_stream(cfg_filepath);
    if(!config_stream.is_open())
        throw std::runtime_error("Failed to open configuration file " + cfg_filepath);
    configuration_t config {};
    config = read_conf(config_stream);
    config_stream.close();
    return config;
}

configuration_t read_conf(std::istream& cf)
{
    std::ios::fmtflags flags( cf.flags() ); // Save stream state
    cf.exceptions(std::ifstream::failbit); // Enable exception on fail

    configuration_t res{};
    std::string temp;

    //filling structure and throwing errors if needed
    try
    {
        std::vector<std::string> vector_names(4);

        for (size_t i = 0; i < vector_names.size(); i++)
        {
            std::getline(cf, vector_names[i]);
            if (vector_names[i].length() == 0)
            {
                throw no_line_exception();
            }
            if (std::count(vector_names[i].begin(), vector_names[i].end(), '\"') != 2)
            {
                if (i != 3)
                {
                    throw no_quotes_exception();
                }
            }
            if (vector_names[i].find('\"',vector_names[i].find('\"') + 1) - vector_names[i].find('\"') == 1)
            {
                if (i != 3)
                {
                    throw invalid_name_exception();
                }
            }
            //extracting needed info from whole string
            vector_names[i] = vector_names[i].substr(vector_names[i].find('=') + 1, vector_names[i].length());
            if (i != 3)
            {
                vector_names[i] = vector_names[i].substr(1, vector_names[i].length() - 2);
            }
            if ((i != 3) && (vector_names[i].find('.') == std::string::npos))
            {
                throw invalid_name_exception();
            }
            else if (i == 3)
            {
                if (vector_names[i].length() == 0)
                {
                    throw invalid_name_exception();
                }
                for (char j : vector_names[i])
                {
                    if (!isdigit(j))
                    {
                        throw invalid_name_exception();
                    }
                }
            }
        }

        for (int j = 0; j < 3; j++)
        {
            if(!boost::filesystem::exists(vector_names[j]))
            {
                throw std::runtime_error("Failed to locate file: " + vector_names[j]);
            }
        }

        if (boost::filesystem::is_empty(vector_names[0]))
        {
            throw std::runtime_error("Input file " + vector_names[0] + " is empty!");
        }

        if ((std::stoi(vector_names[3]) < 1) || (std::stoi(vector_names[3]) > 100))
        {
            throw std::runtime_error("Invalid number of threads");
        }

        res.in_file = vector_names[0];
        res.out_file1 = vector_names[1];
        res.out_file2 = vector_names[2];
        res.threads = std::stoi(vector_names[3]);
    }catch(std::ios_base::failure &fail)
    {
        cf.flags(flags);
        throw std::runtime_error("To Few Parameters");
    }
    catch(invalid_name_exception& e)
    {
        throw std::runtime_error(e.what());
    }
    catch(no_quotes_exception& e)
    {
        throw std::runtime_error(e.what());
    }
    catch(no_line_exception& e)
    {
        throw std::runtime_error(e.what());
    }
    cf.flags(flags);
    return res;
}
